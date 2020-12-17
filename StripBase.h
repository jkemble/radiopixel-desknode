#ifndef STRIPBASE_H
#define STRIPBASE_H

#include <vector>
#include <QtGlobal>
#include <QObject>
#include <QColor>

class StripBase : public QObject
{
    Q_OBJECT;

public:
    StripBase( uint16_t pixels, uint8_t /*pin*/, uint8_t /*type*/ )
        : m_pixels( pixels )
    {
    }

    uint16_t numPixels( ) const
    {
        return m_pixels.size();
    }

    static uint32_t Color( uint8_t r, uint8_t g, uint8_t b )
    {
        return qRgb( r, g, b );
    }

    uint32_t getPixelColor( uint16_t pixel ) const
    {
        return m_pixels[ pixel ];
    }

    void setPixelColor( uint16_t pixel, uint32_t color )
    {
        m_pixels[ pixel ] = color;
    }

    uint8_t getBrightness( ) const
    {
        return m_bright;
    }

    void setBrightness( uint8_t bright )
    {
        m_bright = bright;
    }

signals:
    void show();

private:
    typedef std::vector< uint32_t > Buffer;

    Buffer m_pixels;

    uint8_t m_bright;
};

#endif // STRIPBASE_H
