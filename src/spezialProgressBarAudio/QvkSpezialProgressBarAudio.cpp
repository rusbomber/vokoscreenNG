
#include "QvkSpezialProgressBarAudio.h"
#include "QvkSpezialCheckbox.h"

#include <QDebug>
#include <QCheckBox>
#include <QList>
#include <QScrollArea>
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QBrush>

QvkSpezialProgressBarAudio::QvkSpezialProgressBarAudio( QProgressBar *parent ) : QProgressBar( parent )
{
}


void QvkSpezialProgressBarAudio::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event );

    QList<QScrollArea *> list = this->parent()->parent()->parent()->parent()->findChildren<QScrollArea *>();
    QScrollArea *scrollArea = list.at(0);
    QColor colorFont = scrollArea->palette().windowText().color();
    QColor colorBackground = scrollArea->palette().window().color();

    QPixmap pixmap( width(), height() );
    pixmap.fill( Qt::transparent );

    QPainter painterPixmap;
    {
        painterPixmap.begin( &pixmap );
        painterPixmap.setRenderHint( QPainter::Antialiasing, true );
        painterPixmap.setRenderHint( QPainter::SmoothPixmapTransform, true );

        int lineWidth = 1;
        int fontBaseline = 13;

        // Frame
        QPen pen;
        pen.setColor( Qt::lightGray );
        pen.setStyle( Qt::SolidLine );
        pen.setWidth( lineWidth );
        painterPixmap.setPen( pen );
        QBrush brush;
        brush.setColor( colorBackground );
        brush.setStyle( Qt::SolidPattern );
        painterPixmap.setBrush( brush );
        painterPixmap.drawRect( 0, 0, width(), height() );

        // Bar
        brush.setColor( QStringLiteral( "#3DAEE9" ) );
        brush.setStyle( Qt::SolidPattern );
        painterPixmap.setBrush( brush );
        qreal w = width()-2*lineWidth;
        qreal m = maximum();
        qreal v = value();
        painterPixmap.drawRect( lineWidth, height()-4, w/m*v, height() );

        // Text
        pen.setColor( colorFont );
        painterPixmap.setPen( pen );
        painterPixmap.drawText( 2, fontBaseline, text );

        painterPixmap.end();
    }

    QPainter painter( this );
    painter.drawPixmap( 0, 0, pixmap );
}


void QvkSpezialProgressBarAudio::set_Text( QString m_text ) {
    text = m_text;
}


QString QvkSpezialProgressBarAudio::get_Text()
{
    return text;
}