#include <cmath>
#include <utility>
#include <QApplication>
#include <QHostAddress>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include "radiopixel_protocol.h"
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_strip( STRIP_LENGTH, 0, 0 ),
      m_recvSequence( &m_recvPacket )
{
    // when strip signals, we repaint
    connect( &m_strip, SIGNAL( show()),
             this, SLOT( update()));

    // connect the LAN socket
    m_lanSocket.bind( HN_PORT, QAbstractSocket::ShareAddress );
    connect( &m_lanSocket, SIGNAL( readyRead()),
             this, SLOT( onLanRecv()));

    // connect the cloud socket
    connect( &m_cloudSocket, SIGNAL( readyRead()),
             this, SLOT( onCloudRecv()));
    QTimer *connectTimer = new QTimer( this );
    connect( connectTimer, SIGNAL(timeout()),
             this, SLOT(onCloudReconnect()));
    connectTimer->start( 30 * 1000 );
    onCloudReconnect();

    // start a timer
    startTimer( 25 );

    // start idle pattern
    m_player.SetSequence( &m_idle );

    // restore saved geometry
    if ( !( QApplication::keyboardModifiers() & Qt::ControlModifier ) )
    {
        QSettings settings;
        restoreGeometry( settings.value("geometry").toByteArray( ) );
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::onLanRecv()
{
    while ( m_lanSocket.hasPendingDatagrams())
    {
        m_lanSocket.readDatagram( ( char *)&m_recvPacket, sizeof m_recvPacket );
        m_player.SetSequence( &m_recvSequence );
    }
}

void MainWindow::onCloudReconnect()
{
    if ( m_cloudSocket.state() == QTcpSocket::UnconnectedState )
    {
        m_cloudSocket.connectToHost( "hats.blynch.net", 8100 );
    }
}

void MainWindow::onCloudRecv()
{
    while ( m_cloudSocket.bytesAvailable())
    {
        m_cloudSocket.read( ( char *)&m_recvPacket, sizeof m_recvPacket );
        m_player.SetSequence( &m_recvSequence );
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    ms_t now = millis( );

    if ( m_player.UpdatePattern( now, &m_strip ) )
    {
        //lastTransmit = 0;
    }
    m_player.UpdateStrip( now, &m_strip );
}

int srgbToLinear( int in )
{
    static std::vector<int> lut;
    if ( lut.empty())
    {
        lut.resize( 256 );
        for ( int i = 0; i < 256; ++i )
        {
            if ( i < 1 )
            {
                lut[ i ] = 0;
            }
            else
            {
                double din( i / 255.0 );
                double dout( 1.055 * std::pow( din, 1/2.4) - 0.055 );
                lut[ i ] = dout * 255;
            }
        }
    }

    in = std::min( in, (int)(lut.size() - 1));

    return lut[ in ];
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p( this );

    p.fillRect( rect(), QBrush( QColor( 0, 0, 0)));

    const int sz = 30;
    int w( width() / sz);
    int h( height() / sz);

    uint8_t bright( m_strip.getBrightness());
    uint16_t pixels( m_strip.numPixels());
    for ( int x = 0; x < w; x++ )
    {
        for ( int y = 0; y < h; y++ )
        {
            int pixel = ( w > h ) ? ( y * w + x ) : ( x * h + y );

            QRgb color( m_strip.getPixelColor( pixel % pixels ));
            QRgb mult( Stripper::ColorFade( color, bright));
            QRgb scaled( qRgb( srgbToLinear(qRed(mult)), srgbToLinear(qGreen(mult)), srgbToLinear(qBlue(mult))));
            p.setBrush( QBrush( QColor( scaled)));
            p.drawRect( sz * x, sz * y, sz, sz );
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue( "geometry", saveGeometry());
}
