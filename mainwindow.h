#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QUdpSocket>
#include <QTcpSocket>
#include <QMainWindow>

#include "Player.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLanRecv();
    void onCloudReconnect();
    void onCloudRecv();

private:
    const int STRIP_LENGTH = 92;

    Stripper m_strip;

    // patterns

    Player m_player;
    IdleSequence m_idle;
    AlertSequence m_alert;
    RandomSequence m_randm;

    QUdpSocket m_lanSocket;

    QTcpSocket m_cloudSocket;

    RadioPixel::Command m_recvPacket; // last packet received
    PacketSequence m_recvSequence;
};
#endif // MAINWINDOW_H
