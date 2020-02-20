#ifndef QSNAKE_HH
#define QSNAKE_HH

#include <QMainWindow>
#include "game.hh"
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class QSnake; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	
public slots:
	void timeStep();
	bool eventFilter(QObject* watched, QEvent* event );

private slots:
	void on_actionEasy_triggered();
	
	void on_actionMedium_triggered();
	
	void on_actionHard_triggered();
	
	void on_actionExtreme_triggered();
	
	void on_actionRestore_Defaults_triggered();
	
	void on_actionselfcollision_triggered(bool checked);
	
private:
	enum Difficulty{Easy, Medium, Hard, Extreme};
	
	void setScore();
	void setDifficulty(Difficulty d);

private:
	Ui::QSnake *ui;
	
	Game* game;
	QTimer* timer;
	QLabel* score;
	int delay_ms = 250;
};
#endif // QSNAKE_HH
