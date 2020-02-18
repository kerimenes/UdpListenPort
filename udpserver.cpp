#include "udpserver.h"

UdpServer::UdpServer(QObject *parent)
	: QObject(parent)
{
	qDebug() << "Start Server";
	udpsocket = new QUdpSocket();
	qDebug() << "bind state " << udpsocket->bind(QHostAddress::Any, 17234);
	connect(udpsocket, SIGNAL(readyRead()), SLOT(readPendingDatagrams()));
	connect(udpsocket, SIGNAL(connected()), SLOT(connected()));
}

void UdpServer::connected()
{
	qDebug() << udpsocket->state() << "~~~";
}

void UdpServer::readPendingDatagrams()
{
	qDebug() << "reading Data";
	while(udpsocket->hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(udpsocket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		udpsocket->readDatagram(datagram.data(), datagram.size(),
								&sender, &senderPort);
		QByteArray data;
		data.append(datagram);
		data.append("\n");
		data.append(sender.toString());
		emit newDatagram(data);
	}
}
