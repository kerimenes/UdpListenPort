#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "memorycardhelper.h"

#include <QLabel>
#include <QTimer>
#include <QLineEdit>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QSqlTableModel>

struct TableList{
	int device = 0;
	int card_type = 1;
	int Mac_number = 2;
	int Ip_address = 3;
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	deviceName = "";
	sudoPassword = getPassword();
	mountPath = createMountPath();
	ui->setupUi(this);
	udp = new UdpServer();
	connect(udp, SIGNAL(newDatagram(QByteArray)), SLOT(gettingData(QByteArray)));
	connect(this, SIGNAL(refreshToTable(QString, QString, QString)), SLOT(refreshToTableWidget(QString, QString, QString)));

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget_2->setColumnCount(4);

	mcHelper = new MemoryCardHelper(this);
	connect(mcHelper, SIGNAL(deviceAdded(QString)), this, SLOT(deviceAdded(QString)));
	connect(mcHelper, SIGNAL(deviceRemoved(QString)), SLOT(deviceRemoved(QString)));
	connect(mcHelper, SIGNAL(atachedDevice(QHash<QString, QString>)), SLOT(atachedDevice(QHash<QString, QString>)));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showRightClickMenu(QPoint)));

	timer = new QTimer();
	timer->start(1000);
	connect(timer, SIGNAL(timeout()), SLOT(timeout()));

}

MainWindow::~MainWindow()
{
	delete ui;
}

static void elWait(int msecs)
{
	QElapsedTimer t;
	t.start();
	while (t.elapsed() < msecs)
		QCoreApplication::processEvents();
}

void MainWindow::timeout()
{
	refreshToDevicesList();
}

QString MainWindow::getPassword()
{
	QFrame *f = new QFrame();
	QVBoxLayout *v = new QVBoxLayout(f);

	QLabel *l = new QLabel();
	l->setText("Password");

	QLineEdit *le = new QLineEdit();
	connect(le, SIGNAL(returnPressed()), SLOT(getUserPass()));

	v->addWidget(l);
	v->addWidget(le);
	f->show();

	while(sudoPassword.isEmpty()) {
		QApplication::processEvents();
	}
	f->close();
	QFile file("/var/pass.txt");
	if(file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		file.write(sudoPassword.toLatin1());
	}
	return sudoPassword;
}

void MainWindow::atachedDevice(QHash<QString, QString> devices)
{
	foreach (QString device, devices.keys()) {
		devicesHash.insert(device, devices.value(device));
	}

}

void MainWindow::getUserPass()
{
	QLineEdit* le = qobject_cast<QLineEdit*>(sender());
	sudoPassword = le->text();
}

void MainWindow::showRightClickMenu(QPoint p)
{
	QMenu contextMenu("Context Menu", this);
	QAction action1("Mac Yukle", this);
	QAction action2 ("Kart Çıkart", this);
	QAction action3 ("Mount et", this);
	contextMenu.addActions(QList<QAction*> () << &action1 << &action2 << &action3);
	connect(&action1, SIGNAL(triggered(bool)), SLOT(loadMac(bool)));
	connect(&action2, SIGNAL(triggered(bool)), SLOT(eject(bool)));
	connect(&action3, SIGNAL(triggered(bool)), SLOT(mount(bool)));
	contextMenu.exec(mapToGlobal(p));
}

#include <QMessageBox>
void MainWindow::loadMac(bool triggered)
{
	if(!triggered) {
		mcHelper->loadMac(mountPath, 100);
	}
}

void MainWindow::eject(bool triggered)
{
	if(!triggered) {
		if(!deviceName.isEmpty())
			mcHelper->unmountDevice(deviceName);
	}
}

void MainWindow::mount(bool triggered)
{
	qDebug() << "asdadsadsa";
	if(!triggered) {
		if(!deviceName.isEmpty())
			mcHelper->mountDevice(deviceName);
	}
}

void MainWindow::deviceAdded(QString dev)
{
	devicesHash.insert(dev, "");
	//	QTableWidget *tw = ui->tableWidget_2;
	//	tw->setRowCount(cntDevice+1);
	//	tw->setItem(cntDevice, 0, new QTableWidgetItem(dev));
	//	TableList tl;
	//	if(dev.contains("2")) {
	//		tw->setItem(cntDevice, tl.Mac_number, new QTableWidgetItem(
	//						mcHelper->getMacAddressNumber(dev, sudoPassword, mountPath)));
	//		tw->setItem(cntDevice, tl.Ip_address,new QTableWidgetItem(
	//						mcHelper->getSdCardInfo(mountPath).IP));
	//		tw->setItem(cntDevice, tl.card_type, new QTableWidgetItem(
	//						mcHelper->getSdCardInfo(mountPath).burnmac));
	//	}
	//	cntDevice = cntDevice +1;
	//	devList << dev;
}

static qint8 mountState = 1;
void MainWindow::deviceRemoved(QString dev)
{
	devicesHash.remove(dev);
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
	QString state = value.split(":").first();
	QString percent = value.remove(QString("%1:").arg(state));
	qDebug() << "ipaddr" << ipaddr << "state" << state << "percent" << percent;
	if(!value.isEmpty() & !ipaddr.isEmpty())
		emit refreshToTable(ipaddr, state, percent);
}

void MainWindow::refreshToTableWidget(QString ipaddr, QString state, QString percent)
{
	QTableWidget *table = ui->tableWidget;
	int rows = table->rowCount();
	table->setColumnCount(3);
	QString text;
	QString percent_value;
	double value = percent.split(":").last().toDouble();
	value = 100 / value;
	percent_value = QString("%1").arg((int)percent.split(":").first().toDouble() * value);
	for (int i = 0; i < rows+1; i++) {
		if(!table->item(i, 0)) {
			table->setRowCount(rows+1);
			table->setItem(i, 0, new QTableWidgetItem(ipaddr));
			table->setItem(i, 1, new QTableWidgetItem(state));
			table->setItem(i, 2, new QTableWidgetItem(percent_value));
		} else {
			text = table->item(i, 0)->text();
			if(text == ipaddr) {
				table->setItem(i, 1, new QTableWidgetItem(state));
				table->setItem(i, 2, new QTableWidgetItem(percent_value));
				break;
			}
		}
	}
}

void MainWindow::refreshToDevicesList()
{
	qDebug() << "refreshToDevicesList";
	QTableWidget *table = ui->tableWidget_2;
	if(!devicesHash.empty()) {
		int row_count = table->rowCount();
		foreach (QString device, devicesHash.keys()) {
			for(int i = 0; i < row_count+1; i++) {
				if(!table->item(i, 0)) {
					table->setRowCount(row_count +1);
					table->setItem(i, 0, new QTableWidgetItem(device));
					if(!devicesHash.value(device).isEmpty())
						table->setItem(i, 1, new QTableWidgetItem(devicesHash.value(device)));
					else table->setItem(i, 1, new QTableWidgetItem("Not Mounted"));
				} else {
					QString text = table->item(i, 0)->text();
					if(text.contains(device)) {
						break;
					}
				}
			}
		}
	}
	else  {
		int row_count = table->rowCount();
		for(int i = 0 ; i < row_count ;i++) {
			if(table->item(i, 0))
				table->removeRow(i);
		}
	}

}

QProgressBar* MainWindow::startProgressBar(int maxValue)
{
	QProgressBar* pb = new QProgressBar();
	pb->setRange(0, maxValue);
	return pb;
}

void MainWindow::on_tableWidget_2_itemClicked(QTableWidgetItem *item)
{
	if(item->text().contains("dev/sd"))
		deviceName = item->text();
}

void MainWindow::on_tableWidget_2_cellClicked(int row, int column)
{
	QTableWidget *table = ui->tableWidget_2;
	QString tmp = table->item(row, 0)->text();
	if(tmp.contains("dev/sd"))
		deviceName = tmp;
}
