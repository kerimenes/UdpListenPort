#ifndef MEMORYCARDHELPER_H
#define MEMORYCARDHELPER_H

#include <QProcess>
#include <QProgressBar>
#include <QNetworkReply>
#include "other/qdevicewatcher.h"

struct SdCard {
	QString IP;
	QString device;
	QString scripts;
	QString burnmac;
	QString macnumber;
};

class MemoryCardHelper : public QDeviceWatcher
{
	Q_OBJECT
public:
	explicit MemoryCardHelper(QObject *parent = 0);

	QString getMacAddressNumber(QString dev, QString pass, QString mountpath);
	QString getIPAddress(QString dev, QString pass, QString mountpath);
	int unmountDevice(QString device);
	SdCard getSdCardInfo(QString mountpath);
	void loadMac(QString mountpath, int size);
	QWidget *parentWidget();
	void detechAttachedDevice();
	int mountDevice(QString dev);

	void detechConnectedDevice();
signals:
	void atachedDevice(QHash<QString, QString>);
public slots:
protected:
	int processRun(const QString &cmd);
	int cpMac(QString macpath, int size);
	void progressBar(int maxCount);
	void updateProgress(int value);
	QStringList createMountPath();
protected slots:
//	void deviceAdded(QString dev);
	//	void deviceRemoved(QString dev);
	void replyFinished(QNetworkReply *reply);
	void timeout();
private:
	QStringList pluggedDeviceList;
	QStringList unPluggedDeviceList;
	QProcess *p;
	QStringList randomMacList;
	QString mountaddress;
	QProgressBar *pb;
	QTimer *timer;
	QStringList mountPathList;
	QHash<QString, QString> path1;

};

#endif // MEMORYCARDHELPER_H
