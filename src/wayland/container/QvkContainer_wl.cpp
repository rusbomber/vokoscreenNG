#include "QvkContainer_wl.h"

#include <QStringList>
#include <QList>
#include <QSysInfo>

/*!
 * Hint:
 * Constructor for any format.
 * QString muxer = "matroskamux".
 * QString suffix = "mkv"
 */
Container_wl::Container_wl( QString muxer, QString suffix )
{
    Container_wl::muxer = muxer;
    Container_wl::suffix = suffix;
}

QString Container_wl::get_Muxer()
{
    return muxer;
}

QString Container_wl::get_Suffix()
{
    return suffix;
}

bool Container_wl::get_Available()
{
    return available;
}

/*!
 * Set the muxer to available or unavailable.
 */
void Container_wl::set_MuxerAvailable( bool available )
{
    Container_wl::available = available;
}

void Container_wl::add_VideoCodec( QString encoder, QString name )
{
    VideoCodec_wl videoCodec;
    videoCodec.encoder = encoder;
    videoCodec.name = name;
    VideoCodecs.append( videoCodec );
}

void Container_wl::add_AudioCodec( QString encoder, QString  name )
{
    AudioCodec_wl audioCodec;
    audioCodec.encoder = encoder;
    audioCodec.name = name;
    AudioCodecs.append( audioCodec );
}

void Container_wl::set_VideoCodecAvailable( QString encoder, bool available )
{
    for ( int i = 0; i < VideoCodecs.count(); i++ ) {
        if ( VideoCodecs.at(i).encoder == encoder ) {
            VideoCodec_wl videoCodec = VideoCodecs.at(i);
            videoCodec.available = available;
            VideoCodecs.replace( i, videoCodec );
        }
    }
}

void Container_wl::set_AudioCodecAvailable( QString encoder, bool available )
{
    for ( int i = 0; i < AudioCodecs.count(); i++ ) {
        if ( AudioCodecs.at(i).encoder == encoder ) {
            AudioCodec_wl audioCodec = AudioCodecs.at(i);
            audioCodec.available = available;
            AudioCodecs.replace( i, audioCodec );
        }
    }
}

// ---------------------------------------------------------

/*
 * This is the base for supported container, audio and video codec
 */
QvkContainer_wl::QvkContainer_wl(QObject *parent) : QObject(parent)
{
    Container_wl *MKV = new Container_wl( "matroskamux", "mkv" );
    MKV->add_VideoCodec( "openh264enc", "H.264" );
    MKV->add_AudioCodec( "opusenc", "opus" );
    MKV->add_AudioCodec( "lamemp3enc", "mp3" );

    Containers = new QList<Container_wl*>;
    Containers->append( MKV );
}

/*!
 * Return all supported container like mkv, avi, ... as QList.
 */
QList<Container_wl*> QvkContainer_wl::get_Containers()
{
    return *Containers;
}


/*!
 *  Return all supported video-encoder as QStringList
 */
QStringList QvkContainer_wl::get_AllVideoEncoders()
{
    QList<Container_wl::VideoCodec_wl> list;
    for ( int i = 0; i < Containers->count(); i++ ) {
        list << Containers->at(i)->VideoCodecs;
    }

    QStringList stringList;
    for ( int i = 0; i < list.count(); i++ ) {
        if ( stringList.contains( list.at(i).encoder ) == false ) {
            stringList << list.at(i).encoder;
        }
    }

    return stringList;
}


/*!
 *  Return all supported audio-encoder as QStringList
 */
QStringList QvkContainer_wl::get_AllAudioEncoders()
{
    QList<Container_wl::AudioCodec_wl> list;
    for ( int i = 0; i < Containers->count(); i++ ) {
        list << Containers->at(i)->AudioCodecs;
    }

    QStringList stringList;
    for ( int i = 0; i < list.count(); i++ ) {
        if ( stringList.contains( list.at(i).encoder ) == false ) {
            stringList << list.at(i).encoder;
        }
    }

    return stringList;
}

/*!
 * Input:
 * QString value = "mkv" or "webm" or ...
 *
 * Return:
 * All supported video encoder like vp8enc, avienc, ... as QList
 */
QList<Container_wl::VideoCodec_wl> QvkContainer_wl::get_VideoCodecs( QString suffix )
{
    QList<Container_wl::VideoCodec_wl> list;
    for ( int i = 0; i < Containers->count(); i++ ) {
        if ( Containers->at(i)->get_Suffix() == suffix ) {
            list << Containers->at(i)->VideoCodecs;
        }
    }
    return list;
}


/*!
 * Input:
 * QString value = "mkv" or "webm" or ...
 *
 * Return:
 * All supported audio encoder like vorbisenc, opusenc, ... as QList
 */
QList<Container_wl::AudioCodec_wl> QvkContainer_wl::get_AudioCodecs( QString suffix )
{
    QList<Container_wl::AudioCodec_wl> list;
    for ( int i = 0; i < Containers->count(); i++ ) {
        if ( Containers->at(i)->get_Suffix() == suffix ) {
            list << Containers->at(i)->AudioCodecs;
        }
    }
    return list;
}

/*!
 * Input:
 * Set the container to available or unavailable.
 * QString muxer = "matroskamux".
 */
void QvkContainer_wl::set_ContainerAvailable( QString muxer, bool available  )
{
    for ( int i = 0; i < Containers->count(); i++ ) {
        if ( Containers->at(i)->get_Muxer() == muxer ) {
            Containers->at(i)->set_MuxerAvailable( available );
        }
    }
}
