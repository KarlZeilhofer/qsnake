#ifndef MYFASTERGRAPHICSVIEW_HH
#define MYFASTERGRAPHICSVIEW_HH

#include <QGraphicsView>

// source code from: https://thesmithfam.org/blog/2007/02/03/qt-improving-qgraphicsview-performance/#comment-7215

class MyFasterGraphicsView :public QGraphicsView
{
	Q_OBJECT
public:
	MyFasterGraphicsView(QWidget *parent = Q_NULLPTR);
	
protected:
	
	void paintEvent ( QPaintEvent * event );
	
};



#endif // MYFASTERGRAPHICSVIEW_HH
