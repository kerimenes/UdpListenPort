#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlTableModel>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	udp = new UdpServer();
	connect(udp, SIGNAL(newDatagram(QByteArray)), SLOT(gettingData(QByteArray)));
	connect(this, SIGNAL(refreshToTable(QString,QString)), SLOT(refreshToTableWidget(QString, QString)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::gettingData(QByteArray datagram)
{
	QString ipaddr;
	QString value;
	QString data = QString::fromLatin1(datagram);
	QStringList flds = data.split("\n");
	value = flds.first();
	ipaddr = flds.last();

	ipaddr.remove("::ffff:");
	qDebug() << value << ipaddr << "asdasdsad";
	if(!value.isEmpty() & !ipaddr.isEmpty())
		emit refreshToTable(ipaddr, value);
}

void MainWindow::refreshToTableWidget(QString ipaddr, QString value)
{
	QTableWidget *table = ui->tableWidget;
	int rows = table->rowCount();
	table->setColumnCount(3);
	QString text;
	for (int i = 0; i < rows+1; i++) {
		if(!table->item(i, 0)) {
			table->setRowCount(rows+1);
			table->setItem(i, 0, new QTableWidgetItem(ipaddr));
			table->setItem(i, 1, new QTableWidgetItem(value));
		} else {
			text = table->item(i, 0)->text();
			if(text == ipaddr) {
				table->setItem(i, 1, new QTableWidgetItem(value));
				break;
			}
		}
	}
}
