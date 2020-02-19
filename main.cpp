#include "qsnake.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("QSnake");
	a.setApplicationVersion("0.0.1");
	a.setOrganizationName("HTL Steyr");
	a.setOrganizationDomain("htl-steyr.ac.at");
	QSnake w;
	w.show();
	return a.exec();
}
