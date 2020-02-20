#include "mainwindow.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("QSnake");
	a.setApplicationVersion("0.0.5");
	a.setOrganizationName("KarlZeilhofer");
	a.setOrganizationDomain("zeilhofer.co.at");
	MainWindow w;
	w.show();
	return a.exec();
}
