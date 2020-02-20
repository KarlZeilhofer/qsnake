#ifndef SCENE_HH
#define SCENE_HH

#include <QGraphicsScene>

class Game : public QGraphicsScene
{
public:
	enum Direction{Straight, Left, Right};
	
public:
	Game();
	
	void timeStep(qreal dt);
	void changeDirection(Direction dir);
	void restart();
	void addBrick();
	void addBomb();
	void triggerBomb();
	void paint();
	int score();
	void die();
	void togglePause();
	void setPause(bool flag);
	bool isDead();
	void restoreDefaults();
	void setSelfCollision(bool flag);
	
public slots:
	
private:
	void setDefaults();
	
	// list of snake points in box units (allways positive, >=0)
	// head is first element
	QList<QPointF> snake; 
	bool dead = false;
	
	QList<QPoint> bricks; // filled randomly with obstackles
	QList<QPoint> bombs; // "food" which can be triggerd if needed in a tight situation
	QPoint lastBomb;

	// settings
	qreal BombRadius;
	qreal SizeX; // pixel
	qreal SizeY; // pixel
	qreal BrickSize;// pixel
	qreal SnakeSize;// pixel
	qreal BrickAttraction; // number of retries for finding neighbours, before settling into nowhere
	qreal HeadClearance; // zone around head, forbidden for new bricks
	qreal InitialLength;
	qreal GrowInterval; // seconds
	qreal BombInterval; // seconds
	qreal BrickInterval; // seconds
	
	QColor BombColor;
	QColor SnakeColor1;
	QColor SnakeColor2;
	QColor SnakeHeadColor;
	QColor BackgroundColor;
	QColor BrickColor;
	QColor TextColor;
	
	QStringList valueNames;
	QList<qreal*> values;
	
	qreal currentHeadDirection=0; // degrees, counter clock wise, starting from right
	Direction currentSteering = Straight;

	int bombsCounter=0;
	
	bool paused = false;
	bool selfCollision = false; 
	const int BombFadout = 5;
	int bombFadoutCounter=BombFadout;
	qreal speed = 200; // px/s
};

#endif // SCENE_HH
