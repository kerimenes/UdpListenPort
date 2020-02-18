#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QUdpSocket>

class UdpServer: public QObject
{
	Q_OBJECT
public:
	explicit UdpServer(QObject *parent = 0);

signals:
	void newDatagram(QByteArray data);

public slots:
protected slots:
	void connected();
	void readPendingDatagrams();
private:
	QUdpSocket *udpsocket;
};

#endif // UDPSERVER_H
