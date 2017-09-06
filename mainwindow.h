#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <udpserver.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
signals:
	void refreshToTable(QString ipaddr, QString value);
protected slots:
	void gettingData(QByteArray datagram);
	void refreshToTableWidget(QString ipaddr, QString value);
private:
	Ui::MainWindow *ui;
	UdpServer *udp;
};

#endif // MAINWINDOW_H
