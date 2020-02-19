#ifndef QSNAKE_HH
#define QSNAKE_HH

#include <QMainWindow>
#include "scene.hh"
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class QSnake; }
QT_END_NAMESPACE

class QSnake : public QMainWindow
{
	Q_OBJECT
	
public:
	QSnake(QWidget *parent = nullptr);
	~QSnake();
	
public slots:
	void timeStep();
	bool eventFilter(QObject* watched, QEvent* event );

private slots:
	void on_actionEasy_triggered();
	
	void on_actionMedium_triggered();
	
	void on_actionHard_triggered();
	
	void on_actionExtreme_triggered();
	
private:
	void pause();
	void setScore();
private:
	Ui::QSnake *ui;
	
	Scene* scene;
	QTimer* timer;
	bool paused=false;
	QLabel* score;
	int delay_ms = 250;
};
#endif // QSNAKE_HH
