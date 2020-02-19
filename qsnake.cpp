#include "qsnake.hh"
#include "ui_qsnake.h"

#include "scene.hh"

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QDebug>
#include <QSettings>

QSnake::QSnake(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::QSnake)
{
	ui->setupUi(this);
	scene = new Scene();
	ui->view->setScene(scene);
	
	srand(uint32_t(time(nullptr)));
	
	scene->addItem(new QGraphicsRectItem(50, 50, 60, 40, nullptr));
	
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeStep()));
	timer->start(150);
	
	// catch all events by our own eventFilter()
	qApp->installEventFilter(this);
	
	score = new QLabel("Score: 0", this);
	ui->statusbar->addWidget(score);
	showMaximized();
}

QSnake::~QSnake()
{
	delete ui;
}

void QSnake::timeStep()
{
	if(!paused){
		scene->moveSnake();	
		setScore();
	}
}

bool QSnake::eventFilter(QObject *watched, QEvent *event)
{
	
	if(event->type() == QEvent::KeyPress){
		qDebug() << "key event";
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		
		if( keyEvent->key() == Qt::Key_Up )
		{
			scene->changeDirection(Scene::Up);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_Down )
		{
			scene->changeDirection(Scene::Down);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_Left )
		{
			scene->changeDirection(Scene::Left);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_Right )
		{
			scene->changeDirection(Scene::Right);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_P )
		{
			pause();
			return true;
		}else if( keyEvent->key() == Qt::Key_Space )
		{
			if(scene->isDead()){
				qDebug() << "reset()";
				scene->reset();
			}else {
				scene->triggerBomb();
			}

			return true;
		}else if( keyEvent->key() == Qt::Key_0 )
		{
			QSettings set;
			set.setValue("highScore", 0);
			return true;
		}
	}
	return qApp->eventFilter( watched, event );
}

void QSnake::pause()
{
	paused = !paused;
}

void QSnake::setScore()
{
	QSettings set;
	int hs = set.value("highScore").toInt();
	
	int l = scene->length();
	score->setText(QString("Score: %1    High Score: %2").arg(l).arg(hs));
	
	if(l > hs){
		set.setValue("highScore", l);
	}
}


