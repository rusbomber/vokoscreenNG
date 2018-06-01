#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "ui_QvkNoPlayerDialog.h"

#include <gst/gst.h>

#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QScreen>
#include <QDesktopWidget>
#include <QAudioDeviceInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMimeDatabase>

// gstreamer-plugins-bad-orig-addon
// gstreamer-plugins-good-extra
// libgstinsertbin-1_0-0

#include <gst/gstprotection.h>
#include <glib.h>

static gboolean my_bus_func (GstBus * bus, GstMessage * message, gpointer user_data)
{
   Q_UNUSED(bus);
   Q_UNUSED(user_data);
   GstDevice *device;
   gchar *name;

   // https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstDeviceMonitor.html#gst-device-monitor-get-devices
   // http://gstreamer-devel.966125.n4.nabble.com/Getting-names-and-description-of-the-quot-Source-quot-devices-td2293631.html
   GstElement *pulsesrc;
   g_object_set (G_OBJECT (pulsesrc), "device", device, NULL);
   g_print("11111111111111111111111 %s\n", device);

   switch (GST_MESSAGE_TYPE (message)) {
     case GST_MESSAGE_DEVICE_ADDED:{
       gst_message_parse_device_added (message, &device);
       name = gst_device_get_display_name (device);
       g_print("Device added: %s\n", name);
       g_free (name);
       gst_object_unref (device);
       break; }
     case GST_MESSAGE_DEVICE_REMOVED: {
       gst_message_parse_device_removed (message, &device);
       name = gst_device_get_display_name (device);
       gchar *classes = gst_device_get_device_class (device); // Volker
       g_print("Device removed: %s  %s\n", name, classes);       // Volker
       g_free (name);
       gst_object_unref (device);
       break; }
     default:
       break;
   }

   return G_SOURCE_CONTINUE;
}


GstDeviceMonitor *setup_raw_video_source_device_monitor() // hier darf kein void als Paramter stehen wie im Beispiel vermerkt
{
   GstDeviceMonitor *monitor;
   GstBus *bus;
   GstCaps *caps;

   monitor = gst_device_monitor_new ();

   bus = gst_device_monitor_get_bus (monitor);
   gst_bus_add_watch (bus, my_bus_func, NULL);
   gst_object_unref (bus);

   caps = gst_caps_new_empty_simple ("audio/x-raw");
   gst_device_monitor_add_filter (monitor, "Audio/Source", caps);
   gst_caps_unref (caps);

   // https://developer.gnome.org/programming-guidelines/stable/glist.html.en
   GList *list = gst_device_monitor_get_devices(monitor); //VK
   GList *l;
   gchar *name;
   GstDevice *device;

   g_print( "222222222222222222222222222222\n");
   for (l = list; l != NULL; l = l->next)
     {
       gpointer element_data = l->data;
       //name = gst_device_get_display_name( element_data );
       /* Do something with @element_data. */
     }

   gst_device_monitor_start (monitor);
   g_print( "44444444444444444444444444444444\n");

   return monitor;
}


void cb_fps_measurements(GstElement *fpsdisplaysink,
                         gdouble arg0,
                         gdouble arg1,
                         gdouble arg2,
                         gpointer user_data)
{
       g_print("dropped: %.0f, current: %.2f, average: %.2f\n", arg1, arg0, arg2);
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
//    setup_raw_video_source_device_monitor();

    ui->setupUi(this);

    qDebug().noquote() << VK_GStreamer_Version();

    qDebug() << gst_version_string ();

    ui->tabWidget->setTabIcon( 0, QIcon::fromTheme( "video-display", QIcon( ":/pictures/monitor.png" ) ) );
    makeAndSetValidIcon( 0 );

    ui->tabWidget->setTabIcon( 1, QIcon::fromTheme( "audio-input-microphone", QIcon( ":/pictures/micro.png" ) ) );
    makeAndSetValidIcon( 1 );

    ui->tabWidget->setTabIcon( 2, QIcon::fromTheme( "applications-multimedia", QIcon( ":/pictures/videooptionen.png" ) ) );
    makeAndSetValidIcon( 2 );

    ui->tabWidget->setTabIcon( 3, QIcon::fromTheme( "preferences-system", QIcon( ":/pictures/tools.png" ) ) );
    makeAndSetValidIcon( 3 );

    ui->tabWidget->setTabIcon( 4, QIcon::fromTheme( "camera-web", QIcon( ":/pictures/webcam.png" ) ) );
    makeAndSetValidIcon( 4 );

    regionController = new QvkRegionController();
    regionController->hide();

    vkWinInfo = new QvkWinInfo();

    vkCountdown = new QvkCountdown();

    // Bar for start, stop etc.
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->pushButtonStart,       SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->pushButtonStop,        SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->pushButtonPause,       SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->pushButtonContinue,    SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->pushButtonPlay,        SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->radioButtonFullscreen, SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->radioButtonWindow,     SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->radioButtonArea,       SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->comboBoxScreen,        SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->tabAudio,              SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->tabCodec,              SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), ui->tabMisc,               SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStart, SIGNAL( clicked( bool ) ), this,                      SLOT( slot_preStart() ) );

    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->pushButtonStop,        SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->pushButtonStart,       SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->pushButtonPause,       SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->pushButtonPlay,        SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->radioButtonFullscreen, SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->radioButtonWindow,     SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->radioButtonArea,       SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->comboBoxScreen,        SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->tabAudio,              SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->tabCodec,              SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), ui->tabMisc,               SLOT( setDisabled( bool ) ) );
    connect( ui->pushButtonStop, SIGNAL( clicked( bool ) ), this,                      SLOT( slot_Stop() ) );

    connect( ui->pushButtonPause, SIGNAL( clicked( bool ) ), this,                   SLOT( slot_Pause() ) );
    connect( ui->pushButtonPause, SIGNAL( clicked( bool ) ), ui->pushButtonPause,    SLOT( hide() ) );
    connect( ui->pushButtonPause, SIGNAL( clicked( bool ) ), ui->pushButtonContinue, SLOT( show() ) );
    connect( ui->pushButtonPause, SIGNAL( clicked( bool ) ), ui->pushButtonStop,     SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonPause, SIGNAL( clicked( bool ) ), ui->pushButtonContinue, SLOT( setDisabled( bool ) ) );

    connect( ui->pushButtonContinue, SIGNAL( clicked( bool ) ), this,                   SLOT( slot_Continue() ) );
    connect( ui->pushButtonContinue, SIGNAL( clicked( bool ) ), ui->pushButtonContinue, SLOT( setEnabled( bool ) ) );
    connect( ui->pushButtonContinue, SIGNAL( clicked( bool ) ), ui->pushButtonContinue, SLOT( hide() ) );
    connect( ui->pushButtonContinue, SIGNAL( clicked( bool ) ), ui->pushButtonPause,    SLOT( show() ) );
    connect( ui->pushButtonContinue, SIGNAL( clicked( bool ) ), ui->pushButtonStop,     SLOT( setDisabled( bool ) ) );
    ui->pushButtonContinue->hide();

    connect( ui->pushButtonPlay, SIGNAL( clicked( bool ) ), this, SLOT( slot_Play() ) );

    // Close
    connect( this, SIGNAL( signal_close() ),  ui->pushButtonContinue, SLOT( click() ) );
    connect( this, SIGNAL( signal_close() ),  ui->pushButtonStop, SLOT( click() ) );
    connect( this, SIGNAL( signal_close() ),  regionController, SLOT( close() ) );

    // Tab 1 Screen
    connect( ui->radioButtonArea,   SIGNAL( toggled( bool ) ), regionController,   SLOT( show( bool ) ) );
    connect( ui->radioButtonArea,   SIGNAL( toggled( bool ) ), ui->comboBoxScreen, SLOT( setDisabled( bool ) ) );
    connect( ui->radioButtonWindow, SIGNAL( toggled( bool ) ), ui->comboBoxScreen, SLOT( setDisabled( bool ) ) );

    // Tab 2 Audio
    ui->radioButtonPulse->setAccessibleName( "pulsesrc" );
    ui->radioButtonAlsa->setAccessibleName( "alsasrc" );
    connect( ui->checkBoxAudioOnOff, SIGNAL( toggled( bool ) ), this, SLOT( slot_audioIconOnOff( bool ) ) );
    connect( ui->checkBoxAudioOnOff, SIGNAL( toggled( bool ) ), ui->radioButtonPulse, SLOT( setEnabled( bool ) ) );
    connect( ui->checkBoxAudioOnOff, SIGNAL( toggled( bool ) ), ui->radioButtonAlsa, SLOT( setEnabled( bool ) ) );
    connect( ui->checkBoxAudioOnOff, SIGNAL( toggled( bool ) ), ui->scrollAreaAudioDevice, SLOT( setEnabled( bool ) ) );
    ui->checkBoxAudioOnOff->click();

    connect( ui->radioButtonPulse, SIGNAL( toggled( bool ) ), this, SLOT( slot_clearVerticalLayoutAudioDevices( bool ) ) );
    connect( ui->radioButtonPulse, SIGNAL( toggled( bool ) ), this, SLOT( slot_getPulsesDevices( bool ) ) );
    connect( ui->radioButtonAlsa,  SIGNAL( toggled( bool ) ), this, SLOT( slot_clearVerticalLayoutAudioDevices( bool ) ) );
    connect( ui->radioButtonAlsa,  SIGNAL( toggled( bool ) ), this, SLOT( slot_getAlsaDevices( bool ) ) );

    // Tab 3 Codec
    ui->pushButtonFramesDefault->setIcon ( QIcon::fromTheme( "edit-undo", QIcon( ":/pictures/undo.png" ) ) );
    connect( ui->pushButtonFramesDefault, SIGNAL( clicked( bool ) ), this, SLOT( slot_setFramesStandard( bool ) ) );
    connect( ui->comboBoxFormat, SIGNAL( currentTextChanged( QString ) ), this, SLOT( slot_set_available_VideoCodecs_in_Combox( QString ) ) );
    connect( ui->comboBoxFormat, SIGNAL( currentTextChanged( QString ) ), this, SLOT( slot_set_available_AudioCodecs_in_Combox( QString ) ) );

    // Tab 4 Misc
    videoFileSystemWatcher = new QFileSystemWatcher();
    connect( ui->PushButtonVideoPath, SIGNAL( clicked( bool ) ),        this, SLOT( slot_newVideoPath() ) );
    connect( ui->lineEditVideoPath,   SIGNAL( textChanged( QString ) ), this, SLOT( slot_videoFileSystemWatcherSetNewPath() ) );
    connect( ui->lineEditVideoPath,   SIGNAL( textChanged( QString ) ), this, SLOT( slot_videoFileSystemWatcherSetButtons() ) );
    connect( videoFileSystemWatcher,  SIGNAL( directoryChanged( const QString& ) ), this, SLOT( slot_videoFileSystemWatcherSetButtons() ) );
    ui->lineEditVideoPath->setText( QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ) );

    VK_Supported_Formats_And_Codecs();
    VK_Check_is_Format_available();
    VK_set_available_Formats_in_Combox();

/*
    QDesktopWidget *desk = QApplication::desktop();
    connect( desk, SIGNAL( screenCountChanged(int) ), SLOT( slot_screenCountChanged( int ) ) );
    connect( desk, SIGNAL( resized( int ) ),          SLOT( slot_screenCountChanged( int ) ) );
    emit desk->screenCountChanged(0);
*/
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::slot_newVideoPath()
{
    QString dir = QFileDialog::getExistingDirectory( this,
                                                     "",
                                                     QStandardPaths::writableLocation( QStandardPaths::HomeLocation ),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    if ( dir > "" )
    {
        ui->lineEditVideoPath->setText( dir );
    }
}


void MainWindow::slot_videoFileSystemWatcherSetNewPath()
{
    videoFileSystemWatcher->removePaths( videoFileSystemWatcher->files() );
    videoFileSystemWatcher->addPath( ui->lineEditVideoPath->text() );
}


void MainWindow::slot_videoFileSystemWatcherSetButtons()
{
  QDir dir( ui->lineEditVideoPath->text() );
  QStringList filters;
  filters << "vokoscreen*";
  QStringList List = dir.entryList( filters, QDir::Files, QDir::Time );

  if ( List.isEmpty() || ( ui->pushButtonStart->isEnabled() == false ) )
  {
    ui->pushButtonPlay->setEnabled( false );
  }
  else
  {
    ui->pushButtonPlay->setEnabled( true );
  }
}


void MainWindow::closeEvent( QCloseEvent *event )
{
    Q_UNUSED(event);
    emit signal_close();
}


QString MainWindow::VK_GStreamer_Version()
{
    uint major, minor, micro, nano;
    QString nano_str;

    gst_version(&major, &minor, &micro, &nano);

    if (nano == 1)
      nano_str = "(CVS)";
    else if (nano == 2)
      nano_str = "(Prerelease)";
    else
      nano_str = "";

    QString gstrVersion = QString::number( major ).append( "." )
                 .append( QString::number( minor)).append( "." )
                 .append( QString::number( micro))
                 .append( nano_str );
    return gstrVersion;
}


void MainWindow::makeAndSetValidIcon( int index )
{
  QIcon myIcon = ui->tabWidget->tabIcon( index );
  QSize size = ui->tabWidget->iconSize();
  QPixmap workPixmap( myIcon.pixmap( size ) );
  ui->tabWidget->setTabIcon( index, QIcon( workPixmap ) );
}


/*
 * Setzt neues Icon um aufzuzeigen das Audio abgeschaltet ist
 */
void MainWindow::slot_audioIconOnOff( bool state )
{
  if ( state == Qt::Unchecked )
  {
    QIcon myIcon = ui->tabWidget->tabIcon( 1 );
    QSize size = ui->tabWidget->iconSize();
    QPixmap workPixmap( myIcon.pixmap( size ) );
    QPainter painter;
    QPen pen;
    painter.begin( &workPixmap );
      pen.setColor( Qt::red );
      pen.setWidth( 2 );
      painter.setPen( pen );
      painter.drawLine ( 5, 5, size.width()-5, size.height()-5 );
      painter.drawLine ( 5, size.height()-5, size.width()-5, 5 );
    painter.end();
    ui->tabWidget->setTabIcon( 1, QIcon( workPixmap ) );
  }
  else{
    ui->tabWidget->setTabIcon( 1, QIcon::fromTheme( "audio-input-microphone", QIcon( ":/pictures/micro.png" ) ) );
    makeAndSetValidIcon( 1 );
  }
}


void MainWindow::slot_clearVerticalLayoutAudioDevices( bool value )
{
    Q_UNUSED(value);
    QList<QCheckBox *> listQCheckBox = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>();
    for ( int i = 0; i < listQCheckBox.count(); i++ )
    {
       ui->verticalLayoutAudioDevices->removeWidget( listQCheckBox.at(i) );
       delete listQCheckBox.at(i);
    }
}


void MainWindow::slot_getPulsesDevices( bool value )
{
    Q_UNUSED(value);
    QStringList pulseDeviceStringList;
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices( QAudio::AudioInput ) )
    {
        if ( deviceInfo.deviceName().contains("alsa") == true )
        {
            pulseDeviceStringList << deviceInfo.deviceName();
            QCheckBox *checkboxAudioDevice = new QCheckBox();
            checkboxAudioDevice->setText( deviceInfo.deviceName() );
            checkboxAudioDevice->setAccessibleName( deviceInfo.deviceName() );
            checkboxAudioDevice->setObjectName( "checkboxAudioDevice" + deviceInfo.deviceName() );
            ui->verticalLayoutAudioDevices->insertWidget( ui->verticalLayoutAudioDevices->count()-1, checkboxAudioDevice );
        }
    }
}


void MainWindow::slot_getAlsaDevices( bool value )
{
    Q_UNUSED(value);
    QStringList pulseDeviceStringList;
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices( QAudio::AudioInput ) )
    {
        if ( deviceInfo.deviceName().contains("alsa") == false)
        {
            pulseDeviceStringList << deviceInfo.deviceName();
            QCheckBox *checkboxAudioDevice = new QCheckBox();
            checkboxAudioDevice->setText( deviceInfo.deviceName() );
            checkboxAudioDevice->setAccessibleName( deviceInfo.deviceName() );
            checkboxAudioDevice->setObjectName( "checkboxAudioDevice" + deviceInfo.deviceName() );
            ui->verticalLayoutAudioDevices->insertWidget( ui->verticalLayoutAudioDevices->count()-1, checkboxAudioDevice );
        }
    }
}


void MainWindow::slot_setFramesStandard( bool value )
{
    Q_UNUSED(value);
    ui->spinBoxFrames->setValue( 25 );
}


QString MainWindow::VK_getXimagesrc()
{
    QString showPointer = "true";
    if( ui->checkBoxMouseCursorOnOff->checkState() == Qt::Checked )
    {
        showPointer = "false";
    }

    if( ui->radioButtonFullscreen->isChecked() == true )
    {
        QStringList stringList;
        stringList << "ximagesrc"
                   << "display-name=" + qgetenv( "DISPLAY" )
                   << "use-damage=false"
                   << "show-pointer=" + showPointer;

        QString value = stringList.join( " " );
        return value;
    }

    if( ui->radioButtonWindow->isChecked() == true )
    {
        QStringList stringList;
        stringList << "ximagesrc"
                   << "display-name=" + qgetenv( "DISPLAY" )
                   << "use-damage=false"
                   << "show-pointer=" + showPointer
                   << "xid=" + QString::number( vkWinInfo->getWinID() );

        QString value = stringList.join( " " );
        return value;
    }

    if ( ui->radioButtonArea->isChecked() == true )
    {
        QStringList stringList;
        stringList << "ximagesrc"
                   << "display-name=" + qgetenv( "DISPLAY" )
                   << "use-damage=false"
                   << "show-pointer=" + showPointer
                   << "startx=" + QString::number( regionController->getX() )
                   << "starty=" + QString::number( regionController->getY() )
                   << "endx="   + QString::number( regionController->getX() + regionController->getWidth() )
                   << "endy="   + QString::number( regionController->getY() + regionController->getHeight() );
        QString value = stringList.join( " " );
        return value;
    }

    return "";
}


QString MainWindow::VK_getCapsFilter()
{
   QStringList stringList;
   stringList << "caps=video/x-raw"
              << QString("framerate=") + ui->spinBoxFrames->text() + "/1";
   return QString( "capsfilter" ) + " " + stringList.join( "," );
}


void MainWindow::VK_Supported_Formats_And_Codecs()
{
    QStringList MKV_QStringList = ( QStringList()
                                    << "muxer:matroskamux"
                                    << "suffix:mkv"
                                    << "videomimetype:video/x-matroska"
                                    << "audiomimetype:audio/x-matroska"
                                    << "videocodec:x264enc:x264"
                                    << "videocodec:x265enc:x265"
                                    << "videocodec:theoraenc:theora"
                                    << "videocodec:vp8enc:vp8"
                                    << "videocodec:vp9enc:vp9"
                                    << "audiocodec:vorbisenc:vorbis"
                                    << "audiocodec:flacenc:flac"
                                    << "audiocodec:opusenc:opus"
                                   );

    QStringList WEBM_QStringList = ( QStringList()
                                     << "muxer:webmmux"
                                     << "suffix:webm"
                                     << "videomimetype:video/webm"
                                     << "audiomimetype:audio/webm"
                                     << "videocodec:vp8enc:vp8"
                                     << "videocodec:vp9enc:vp9"
                                     << "audiocodec:vorbisenc:vorbis"
                                   );

    videoFormatsList.clear();
    videoFormatsList.append( MKV_QStringList.join( ","  ) );
    videoFormatsList.append( WEBM_QStringList.join( ","  ) );


    /*
        videoFormatsList.append( "mp4mux:mp4" );
        videoFormatsList.append( "avimux:avi" );
        videoFormatsList.append( "asfmux:asf" );
        videoFormatsList.append( "flvmux:flv" );
    */

}


void MainWindow::VK_Check_is_Format_available()
{
    QStringList tempList;
    for ( int x = 0; x < videoFormatsList.count(); x++ )
    {
        QString stringAllKeys = videoFormatsList.at( x );
        QStringList listKeys = stringAllKeys.split( "," );
        QStringList listKey = listKeys.filter( "muxer");
        GstElementFactory *factory = gst_element_factory_find( QString( listKey.at( 0 ) ).mid( 6 ).toLatin1() );
        if ( !factory )
        {
            qDebug() << "Failed to find factory of type:" << QString( listKey.at( 0 ) ).mid( 6 ).toLatin1();
        }
        else
        {
            tempList << videoFormatsList.at( x );
        }
    }
    videoFormatsList.clear();
    videoFormatsList << tempList;
}


void MainWindow::VK_set_available_Formats_in_Combox()
{
    ui->comboBoxFormat->clear();

    for ( int x = 0; x < videoFormatsList.count(); x++  )
    {
        QString stringAllKeys = videoFormatsList.at(x);
        QStringList listKeys = stringAllKeys.split( "," );
        QStringList listKeySuffix = listKeys.filter( "suffix");
        QStringList listKeyMuxer = listKeys.filter( "muxer" );

        QMimeDatabase mimeDatabase;
        QStringList listKeyVideoMimetype = listKeys.filter( "videomimetype" );
        int y = QString( listKeyVideoMimetype.at(0) ).indexOf( ":" );
        QMimeType mimetype = mimeDatabase.mimeTypeForName( listKeyVideoMimetype.at(0).mid(y+1) );
        QIcon icon = QIcon::fromTheme( mimetype.iconName(), QIcon( ":/pictures/videooptionen.png" ) );

        ui->comboBoxFormat->addItem( icon,
                                     QString( listKeySuffix.at( 0 ) ).mid( 7 ),
                                     QString( listKeyMuxer.at( 0 ) ).mid( 6 ) );
    }
}


void MainWindow::slot_set_available_VideoCodecs_in_Combox( QString suffix )
{
    ui->comboBoxVideoCodec->clear();

    QStringList listSuffix = videoFormatsList.filter( suffix );
    QString stringSuffix = listSuffix.at( 0 );
    QStringList listKeys = stringSuffix.split( "," );
    QStringList listKeyVideoCodec = listKeys.filter( "videocodec" );
    for ( int i = 0; i < listKeyVideoCodec.count(); i++ )
    {
        int yfirst = QString( listKeyVideoCodec.at( i ) ).indexOf( ":" );
        int ylast = QString( listKeyVideoCodec.at( i ) ).lastIndexOf( ":" );
        QString encoder = QString(listKeyVideoCodec.at( i )).mid( yfirst + 1, ( ylast - 1 ) - yfirst );
        QString name = QString( listKeyVideoCodec.at( i ) ).mid( ylast + 1 );
        ui->comboBoxVideoCodec->addItem( name, encoder );
    }
}


void MainWindow::slot_set_available_AudioCodecs_in_Combox( QString suffix )
{
    ui->comboBoxAudioCodec->clear();

    QStringList listSuffix = videoFormatsList.filter( suffix );
    QString stringSuffix = listSuffix.at( 0 );
    QStringList listKeys = stringSuffix.split( "," );
    QStringList listKeyVideoCodec = listKeys.filter( "audiocodec" );
    for ( int i = 0; i < listKeyVideoCodec.count(); i++ )
    {
        int y = QString( listKeyVideoCodec.at( i ) ).lastIndexOf( ":" );
        ui->comboBoxAudioCodec->addItem( QString( listKeyVideoCodec.at( i ) ).mid( y + 1 ) );// QString(listKeyVideoCodec.at(i)).mid(11) );
    }
}


QString MainWindow::VK_getMuxer()
{
    GstElementFactory *factory = gst_element_factory_find( "matroskamux" );
    if ( !factory )
    {
      qDebug() << "Failed to find factory of type matroskamux";
    }

    QString device;
    QList<QCheckBox *> listQCheckBox = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>();
    for ( int i = 0; i < listQCheckBox.count(); i++ )
    {
        if ( listQCheckBox.at(i)->checkState() == Qt::Checked )
        {
            device = listQCheckBox.at(i)->accessibleName();
        }
    }

    QString value = "matroskamux";
    if ( ( ui->checkBoxAudioOnOff->isChecked() == true ) and ( device > "" ) )
    {
        value = "mux. matroskamux name=mux";
    }

    return value;
}


void MainWindow::slot_preStart()
{
    if ( ( ui->spinBoxCountDown->value() > 0 ) and ( ui->radioButtonWindow->isChecked() == true )  )
    {
        disconnect( vkWinInfo, 0, 0, 0 );
        disconnect( vkCountdown, 0, 0, 0 );
        connect( vkWinInfo,   SIGNAL( windowChanged( bool ) ),          this,                SLOT( slot_startCounter( bool ) ) );
        connect( vkWinInfo,   SIGNAL( signal_showCursor( bool ) ),      ui->pushButtonStop,  SLOT( setDisabled( bool ) ) );
        connect( vkWinInfo,   SIGNAL( signal_showCursor( bool ) ),      ui->pushButtonPause, SLOT( setDisabled( bool ) ) );
        connect( vkCountdown, SIGNAL( signal_countDownfinish( bool ) ), ui->pushButtonStop,  SLOT( setEnabled( bool ) ) );
        connect( vkCountdown, SIGNAL( signal_countDownfinish( bool ) ), ui->pushButtonPause, SLOT( setEnabled( bool ) ) );
        connect( vkCountdown, SIGNAL( signal_countDownfinish( bool ) ), this,                SLOT( slot_Start() ) );
        vkWinInfo->slot_start();
        return;
    }

    if ( ui->spinBoxCountDown->value() > 0 )
    {
        disconnect( vkCountdown, 0, 0, 0 );
        connect( vkCountdown, SIGNAL( signal_countdownBegin( bool ) ),  ui->pushButtonStop,  SLOT( setDisabled( bool ) ) );
        connect( vkCountdown, SIGNAL( signal_countdownBegin( bool ) ),  ui->pushButtonPause, SLOT( setDisabled( bool ) ) );
        connect( vkCountdown, SIGNAL( signal_countDownfinish( bool ) ), ui->pushButtonStop,  SLOT( setEnabled( bool ) ) );
        connect( vkCountdown, SIGNAL( signal_countDownfinish( bool ) ), ui->pushButtonPause, SLOT( setEnabled( bool ) ) );
        connect( vkCountdown, SIGNAL( signal_countDownfinish( bool ) ), this,                SLOT( slot_Start() ) );
        vkCountdown->startCountdown( ui->spinBoxCountDown->value() );
        return;
    }

    if ( ui->radioButtonWindow->isChecked() == true )
    {
        disconnect( vkWinInfo, 0, 0, 0 );
        connect( vkWinInfo, SIGNAL( signal_showCursor( bool ) ), ui->pushButtonStop,  SLOT( setDisabled( bool ) ) );
        connect( vkWinInfo, SIGNAL( signal_showCursor( bool ) ), ui->pushButtonPause, SLOT( setDisabled( bool ) ) );
        connect( vkWinInfo, SIGNAL( windowChanged( bool ) ),     ui->pushButtonStop,  SLOT( setEnabled( bool ) ) );
        connect( vkWinInfo, SIGNAL( windowChanged( bool ) ),     ui->pushButtonPause, SLOT( setEnabled( bool ) ) );
        connect( vkWinInfo, SIGNAL( windowChanged( bool ) ),     this,                SLOT( slot_Start() ) );
        vkWinInfo->slot_start();
        return;
    }

    slot_Start();
}


void MainWindow::slot_startCounter( bool value )
{
    Q_UNUSED(value);
    if ( ui->spinBoxCountDown->value() > 0 )
    {
        vkCountdown->startCountdown( ui->spinBoxCountDown->value() );
    }
}


void MainWindow::slot_Start()
{
    GstElementFactory *factory = gst_element_factory_find( "ximagesrc" );
    if ( !factory )
    {
        qDebug() << "Failed to find factory of type ximagesrc";
        return;
    }

    QString filename = "vokoscreen-" + QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh-mm-ss" ) + "." + "mkv";
    QString path = ui->lineEditVideoPath->text();

    QStringList VK_PipelineList;
    VK_PipelineList << VK_getXimagesrc();
    VK_PipelineList << VK_getCapsFilter();
    VK_PipelineList << "queue flush-on-eos=true";
    VK_PipelineList << "videoconvert";
    VK_PipelineList << "videorate";
    VK_PipelineList << "x264enc speed-preset=veryfast pass=quant threads=0";

    QString device;
    QList<QCheckBox *> listQCheckBox = ui->scrollAreaWidgetContents->findChildren<QCheckBox *>();
    for ( int i = 0; i < listQCheckBox.count(); i++ )
    {
        if ( listQCheckBox.at(i)->checkState() == Qt::Checked )
        {
            device = listQCheckBox.at(i)->accessibleName();
        }
    }

    QString audioSystem;
    if ( ui->radioButtonPulse->isChecked() )
    {
        audioSystem = ui->radioButtonPulse->accessibleName();
    }
    if ( ui->radioButtonAlsa->isChecked() )
    {
        audioSystem = ui->radioButtonAlsa->accessibleName();
    }

    if ( ( ui->checkBoxAudioOnOff->isChecked() == true ) and ( device > "" ) )
    {
        VK_PipelineList << QString( "mux. ").append( audioSystem ).append( " device=" ).append( device );
        VK_PipelineList << "audioconvert";
        VK_PipelineList << "audiorate";
        VK_PipelineList << "voaacenc";
        VK_PipelineList << "queue flush-on-eos=true";
    }

    VK_PipelineList << VK_getMuxer();
    VK_PipelineList << "filesink location=" + path + QDir::separator() + filename;

    QString VK_Pipeline = VK_PipelineList.join( VK_Gstr_Pipe );
    qDebug() << "[vokoscreen] Start record with:" << VK_Pipeline;
    pipeline = gst_parse_launch( VK_Pipeline.toLatin1(), &error );
/*
    g_print( error->message );

    GstElement *fpsdisplaysink;
    //myPipeline = gst_parse_launch( "ximagesrc use-damage=false ! capsfilter caps=video/x-raw ! videoconvert ! fpsdisplaysink video-sink=fakesink signal-fps-measurements=true name=sink", &error );
    fpsdisplaysink = gst_bin_get_by_name (GST_BIN(pipeline), "sink" );
    if (fpsdisplaysink)
    {
        qDebug() << "*********************************************************************************";
        g_object_set (G_OBJECT (fpsdisplaysink), "signal-fps-measurements", TRUE, NULL);
        g_signal_connect (fpsdisplaysink, "fps-measurements", G_CALLBACK (cb_fps_measurements), NULL);
        g_print("fps-measurements connected\n");
    }
*/

    // Start playing
    GstStateChangeReturn ret;
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr ("[vokoscreen] Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return;
    }
}


void MainWindow::slot_Stop()
{
    {
        // wait for EOS
        bool a = gst_element_send_event (pipeline, gst_event_new_eos());
        Q_UNUSED(a);

        GstClockTime timeout = 10 * GST_SECOND;
        GstMessage *msg = gst_bus_timed_pop_filtered (GST_ELEMENT_BUS (pipeline), timeout, GST_MESSAGE_EOS );
        Q_UNUSED(msg);

        GstStateChangeReturn ret ;
        Q_UNUSED(ret);
        ret = gst_element_set_state( pipeline, GST_STATE_PAUSED );
        ret = gst_element_set_state( pipeline, GST_STATE_READY );
        ret = gst_element_set_state( pipeline, GST_STATE_NULL );
        gst_object_unref( pipeline );
        qDebug() << "[vokoscreen] Stop record";
    }
}


void MainWindow::slot_Pause()
{
    if ( ui->pushButtonStart->isEnabled() == false )
    {
        qDebug() << "[vokoscreen] Pause was clicked";
        GstStateChangeReturn ret = gst_element_set_state( pipeline, GST_STATE_PAUSED );
        Q_UNUSED(ret);
    }
}


void MainWindow::slot_Continue()
{
    if ( ( ui->pushButtonStart->isEnabled() == false ) and ( ui->pushButtonContinue->isEnabled() == true ) )
    {
        qDebug() << "[vokoscreen] Continue was clicked";
        GstStateChangeReturn ret = gst_element_set_state( pipeline, GST_STATE_PLAYING );
        Q_UNUSED(ret);
    }
}


void MainWindow::slot_Play()
{
    qDebug() << "[vokoscreen] play video with standard system player";
    QDir dir( ui->lineEditVideoPath->text() );
    QStringList filters;
    filters << "vokoscreen*";
    QStringList videoFileList = dir.entryList( filters, QDir::Files, QDir::Time );

    QString string;
    string.append( "file://" );
    string.append( ui->lineEditVideoPath->text() );
    string.append( QDir::separator() );
    string.append( videoFileList.at( 0 ) );
    bool b = QDesktopServices::openUrl( QUrl( string, QUrl::TolerantMode ) );
    if ( b == false )
    {
        QDialog *newDialog = new QDialog;
        Ui_NoPlayerDialog myUiDialog;
        myUiDialog.setupUi( newDialog );
        newDialog->setModal( true );
        newDialog->setWindowTitle( "vokoscreen" );
        newDialog->show();
    }
}


void MainWindow::slot_screenCountChanged(int newCount )
{
    Q_UNUSED(newCount);
    ui->comboBoxScreen->clear();
    QDesktopWidget *desk = QApplication::desktop();
    qDebug() << "[vokoscreen]" << "---Begin search Screen---";
    qDebug() << "[vokoscreen]" << "Number of screens:" << desk->screenCount();
    qDebug() << "[vokoscreen] Primary screen is: Display" << desk->primaryScreen()+1;
    qDebug() << "[vokoscreen] VirtualDesktop:" << desk->isVirtualDesktop();

    //QList < QScreen *> screens = QGuiApplication::screens();
    QScreen *screen = QGuiApplication::primaryScreen();
    qDebug() << "[vokoscreen] DevicePixelRatio:" << screen->devicePixelRatio() << " (Normal displays is 1, Retina display is 2)";

    for ( int i = 1; i < desk->screenCount()+1; i++ )
    {
        QString ScreenGeometryX1 = QString::number( desk->screenGeometry( i-1 ).left() );
        QString ScreenGeometryY1 = QString::number( desk->screenGeometry( i-1 ).top() );
        QString ScreenGeometryX = QString::number( desk->screenGeometry( i-1 ).width() * screen->devicePixelRatio() ); // devicePixelRatio() for Retina Displays
        QString ScreenGeometryY = QString::number( desk->screenGeometry( i-1 ).height() * screen->devicePixelRatio() );
        ui->comboBoxScreen->addItem( tr( "Display" ) + " " + QString::number( i ) + ":  " + ScreenGeometryX + " x " + ScreenGeometryY, i-1 );
        qDebug().noquote() << "[vokoscreen]" << "Display " + QString::number( i ) + ":  " + ScreenGeometryX + " x " + ScreenGeometryY;
    }
    ui->comboBoxScreen->addItem( tr( "All Displays" ), -1 );
    qDebug() << "[vokoscreen]" << "---End search Screen---";
    qDebug( " " );
}
