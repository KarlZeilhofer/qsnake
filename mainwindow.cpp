#include "mainwindow.hh"
#include "ui_qsnake.h"

#include "game.hh"

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QDebug>
#include <QSettings>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::QSnake)
{
	ui->setupUi(this);
	game = new Game();
	ui->view->setScene(game);
	
	srand(uint32_t(time(nullptr)));
	
	QActionGroup* ag_difficulty =  new QActionGroup(this);
	ag_difficulty->addAction(ui->actionEasy);
	ag_difficulty->addAction(ui->actionMedium);
	ag_difficulty->addAction(ui->actionHard);
	ag_difficulty->addAction(ui->actionExtreme);
	
	QSettings set;
	
	Difficulty d = Difficulty(set.value("difficulty", 0).toInt());
	
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeStep()));
	setDifficulty(d);
	timer->start();
	
	// catch all events by our own eventFilter()
	qApp->installEventFilter(this);
	
	score = new QLabel("Score: 0", this);
	ui->statusbar->addWidget(score);
	
	setWindowTitle(QApplication::applicationName() + " V" + QApplication::applicationVersion());
	showMaximized();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::timeStep()
{
	game->moveSnake();	
	setScore();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
	
	if(event->type() == QEvent::KeyPress){
		qDebug() << "key event";
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		
		if( keyEvent->key() == Qt::Key_Up )
		{
			game->changeDirection(Game::Up);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_Down )
		{
			game->changeDirection(Game::Down);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_Left )
		{
			game->changeDirection(Game::Left);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_Right )
		{
			game->changeDirection(Game::Right);
			return true;
		}else 	if( keyEvent->key() == Qt::Key_P )
		{
			game->togglePause();
			return true;
		}else if( keyEvent->key() == Qt::Key_Space )
		{
			if(game->isDead()){
				qDebug() << "reset()";
				game->reset();
			}else {
				game->triggerBomb();
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

void MainWindow::setScore()
{
	QSettings set;
	int hs = set.value("highScore").toInt();
	
	int l = game->length();
	score->setText(QString("Score: %1    High Score: %2").arg(l).arg(hs));
	
	if(l > hs){
		set.setValue("highScore", l);
	}
}

void MainWindow::setDifficulty(MainWindow::Difficulty d)
{
	QSettings set;
	
	switch (d) {
	case Easy:
		delay_ms = set.value("delayEasy", 250).toInt();
		set.setValue("delayEasy", delay_ms);
		ui->actionEasy->setChecked(true);
		break;
	case Medium:
		delay_ms = set.value("delayMedium", 150).toInt();
		set.setValue("delayMedium", delay_ms);
		ui->actionMedium->setChecked(true);
		break;
	case Hard:
		delay_ms = set.value("delayHard", 100).toInt();
		set.setValue("delayHard", delay_ms);
		ui->actionHard->setChecked(true);
		break;
	case Extreme:
		delay_ms = set.value("delayExtreme", 50).toInt();
		set.setValue("delayExtreme", delay_ms);
		ui->actionExtreme->setChecked(true);
		break;
	}
	timer->setInterval(delay_ms);
	
	set.setValue("difficulty", int(d));
}



void MainWindow::on_actionEasy_triggered()
{
	setDifficulty(Easy);
}

void MainWindow::on_actionMedium_triggered()
{
	setDifficulty(Medium);
}

void MainWindow::on_actionHard_triggered()
{
	setDifficulty(Hard);
}

void MainWindow::on_actionExtreme_triggered()
{
	setDifficulty(Extreme);
}

void MainWindow::on_actionRestore_Defaults_triggered()
{
    game->restoreDefaults();
}
