#include "myfastergraphicsview.hh"
#include <QPaintEvent>

MyFasterGraphicsView::MyFasterGraphicsView(QWidget *parent)
	:QGraphicsView(parent)
{
	
}

void MyFasterGraphicsView::paintEvent ( QPaintEvent * event )
{
	QPaintEvent *newEvent=new QPaintEvent(event->region().boundingRect());
	QGraphicsView::paintEvent(newEvent);
	delete newEvent;
}
