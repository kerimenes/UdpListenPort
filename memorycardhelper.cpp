#include "memorycardhelper.h"

#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QProcess>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <sys/mount.h>
#include <errno.h>
#include <QTimer>

#define MAX_MAC 10

MemoryCardHelper::MemoryCardHelper(QObject *parent)
	: QDeviceWatcher(parent)
{
	if(start())
		qDebug() << "Start Device Watcher process";
	else
		qDebug() << "Error Device Watch process";
	qDebug() << parentWidget();
	progressBar(MAX_MAC);
	mountPathList = createMountPath();
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), SLOT(timeout()));
	timer->start(1000);
}

QStringList MemoryCardHelper::createMountPath()
{
	QDir d;
	d.mkdir(QDir::currentPath() + "/mnt1");
	d.mkdir(QDir::currentPath() + "/mnt2");
	d.mkdir(QDir::currentPath() + "/mnt3");
	return QStringList () << QDir::currentPath() + "/mnt1" << QDir::currentPath() + "/mnt2" << QDir::currentPath() + "/mnt3";
}

SdCard MemoryCardHelper::getSdCardInfo(QString mountpath)
{
	QDir d;
	QString dirpath;
	SdCard sd;
	QDirIterator directories(d.filePath(mountpath), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	while(directories.hasNext()) {
		QString path = directories.next();
		if(path.contains("etc")) {
			QDirIterator subdir(path, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
			while(subdir.hasNext()) {
				path = subdir.next();
				if(path.contains("network")) {
					dirpath = path + "/interfaces";
					QFile f(dirpath);
					if(!f.open(QIODevice::ReadOnly))
						qDebug() << "opening device " << dirpath;
					QString data = f.readAll().data();
					f.close();
					QString ipState = "NONE";
					QString address = "NONE";
					foreach (QString line, data.split("\n")) {
						if(line.contains("iface eth0"))
							ipState = line.split(" ").last();
						if(line.contains("address"))
							address = line.remove("address ");
					}
					sd.IP = ipState + " " + address;
				} else if (path.contains("init.d")) {
					dirpath = path + "/rcS";
					QFile f(dirpath);
					if(!f.open(QIODevice::ReadOnly))
						qDebug() << "File open error" << dirpath;
					QString data = f.readAll().data();
					f.close();
					foreach (QString line, data.split("\n")) {
						if(line.contains("burnmac")) {
							sd.burnmac = "Mac Adresi yazici";
							break;
						} else sd.burnmac = "False";
					}
				}
			}
		} else continue;
	}
	return sd;
}

void MemoryCardHelper::loadMac(QString mountpath, int size)
{
#if 0
	QDir d;
	QFile f("random_mac_list.txt");
	if(!f.open(QIODevice::ReadWrite | QIODevice::Text))
		qDebug() << "load Mac" << "error open file";

	d.setCurrent(mountpath + "/etc" + "/macs");
	qDebug() << d.absolutePath();
	QTextStream in(&f);
	int cnt = 0;
	while (!in.atEnd()) {
		cnt = cnt + 1;
		if(size == cnt)
			break;
		QString line = in.readLine();
		qDebug() << cnt << line;
		if(!line.contains("00:22:33"))
			continue;
		QFile file(line);
		if(!file.open(QIODevice::WriteOnly)) {
			qDebug() << "error file open mac" << file.errorString();
			continue;
		}
		QTextStream stream(&file);
		stream << " " << endl;
		file.close();
		qDebug() << cnt;
		line.clear();
		in << line << "\n";
	}
	f.close();
#endif
	mountaddress = mountpath;
	int mac_count = 0;
	while (mac_count < MAX_MAC) {
		QNetworkAccessManager *netman = new QNetworkAccessManager();
		connect(netman, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
		mac_count++;
		updateProgress(mac_count);
		netman->get(QNetworkRequest(QUrl("http://10.10.1.1:44715/random_mac?")));
	}

}

QWidget *MemoryCardHelper::parentWidget()
{
	return (QWidget *)parent();
}

void MemoryCardHelper::detechAttachedDevice()
{
	QHash<QString, QString> devices;
	QProcess p;
	p.start("mount");
	p.waitForFinished(300);
	QString data = p.readAllStandardOutput().data();
	foreach (QString line, data.split("\n")) {
		if(line.isEmpty() | !line.contains("/dev/sd"))
			continue;
		if(line.contains("sda"))
			continue;
		QStringList flds = line.split("on");
		devices.insert(flds.first().remove(" "), flds.last().split("type").first().remove(" "));
	}
	if(devices.isEmpty())
		return;
	else emit atachedDevice(devices);
}

void MemoryCardHelper::detechConnectedDevice()
{
	QHash<QString, QString> devices;
	QProcess p;
	p.start("cat /proc/partitions");
	p.waitForFinished(300);
	QString data = p.readAllStandardOutput().data();
	foreach (QString line, data.split("\n")) {
		if(line.isEmpty() | line.contains("sda") | !line.contains("sd"))
			continue;
		QStringList flds = line.split(" ");
		QString device = "/dev/" + flds.last();
		devices.insert(device, "");
	}
	if(devices.isEmpty())
		return;
	else emit atachedDevice(devices);
}

void MemoryCardHelper::replyFinished(QNetworkReply *reply)
{
	QString randomMac = reply->readAll();
	QDir::setCurrent(mountaddress + "/etc" + "/macs");
	QFile f(randomMac);
	if(!f.open(QIODevice::WriteOnly)) {
		qDebug() << "error file open mac" << f.errorString() << randomMac;
		return;
	}
	QTextStream stream(&f);
	stream << " " << endl;
	f.close();
}

void MemoryCardHelper::timeout()
{
	timer->setInterval(1000);
	detechAttachedDevice();
	detechConnectedDevice();
}

int MemoryCardHelper::mountDevice(QString dev)
{
	const char *device;
	const char *path;

	foreach (QString mp, mountPathList) {
		if(path1.values().contains(mp))
			continue;
		else {
			path1.insert(dev, mp);
			QByteArray ba = mp.toLocal8Bit();
			path = ba.data();
			device = dev.toLatin1().data();
		}
	}
	qDebug() << device << path << path1;
	qDebug() << "Buraya kadar normal mount edebilirsin";
//	int status = mount(device, mountpath.toLatin1().data(), "ext2", 0, "");
//	qDebug() << errno << strerror(status);
}

QString MemoryCardHelper::getMacAddressNumber(QString dev, QString pass, QString mountpath)
{
	if(!dev.contains("2"))
		return dev;
	if(!mountDevice(dev)){
		QProcess p;
		p.start(QString("ls -l /%1/etc/macs/").arg(mountpath));
		if(!p.waitForReadyRead())
			return "NONE";
		if(!p.waitForFinished())
			return "NONE1";
		QString data = p.readAll().data();
		QStringList flds = data.split("\n");
		SdCard sd = getSdCardInfo(mountpath);
		return QString::number(flds.size());
	} else return "NONE";
}

int MemoryCardHelper::unmountDevice(QString device)
{
	if(path1.keys().isEmpty()) {
		qDebug() << "we dont have mounted device..";
		return 0;
	}
	char *path;
	foreach (QString key, path1.keys()) {
		if(key == device) {
			QByteArray ba = path1.value(key).toLocal8Bit();
			path = ba.data();
		} else qDebug() << "we dont have mounted device..";
	}

	qDebug() << path;
	qDebug() << "Buraya kadar normal umount edebilirsin";

//	int status = umount(path.toLatin1().data());
//	qDebug() << status << errno << strerror(status);
}

void MemoryCardHelper::progressBar(int maxCount)
{
	pb = new QProgressBar(parentWidget());
	pb->setMinimum(0);
	pb->setMaximum(maxCount);
}

void MemoryCardHelper::updateProgress(int value)
{
	pb->setValue(value);
}
