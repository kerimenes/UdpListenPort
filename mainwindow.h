#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QProgressBar>
#include <QTableWidgetItem>
#include <QString>

#include "udpserver.h"
//#include "memorycardhelper.h"

struct Deneme {

};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void addedMacNumber();
	void addMacNumber();
signals:
	void refreshToTable(QString ipaddr, QString value, QString percent);
protected:
	QProgressBar *startProgressBar(int maxValue);
	QString getPassword();
	QString createMountPath () { QDir d; if(d.mkdir(QString("%1/mnt").arg(d.absolutePath())))
			return QString("%1/mnt").arg(d.absolutePath());
		else return d.filePath(QString("%1/mnt").arg(d.absolutePath()));}

	void refreshToDevicesList();
protected slots:
	void timeout();
	void gettingData(QByteArray datagram);
	void refreshToTableWidget(QString ipaddr, QString state, QString percent);
	void deviceAdded(QString dev);
	void deviceRemoved(QString dev);
	void getUserPass();
	void showRightClickMenu(QPoint p);
	void loadMac(bool triggered);
	void eject(bool triggered);
	void atachedDevice(QHash<QString, QString> devices);
	void mount(bool triggered);
private slots:
	void on_tableWidget_2_itemClicked(QTableWidgetItem *item);

	void on_tableWidget_2_cellClicked(int row, int column);

private:
	Ui::MainWindow *ui;
//	MemoryCardHelper *mcHelper;
	UdpServer *udp;
	int cntDevice = 0;
	QString sudoPassword;
	QString mountPath;
	QStringList devList;
	QHash<QString, QString> devicesHash;
	QHash<QString, QString> devicesInfo;
	QTimer *timer;
	QString deviceName;
};

#endif // MAINWINDOW_H
