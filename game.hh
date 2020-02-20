#ifndef SCENE_HH
#define SCENE_HH

#include <QGraphicsScene>

class Game : public QGraphicsScene
{
public:
	enum Direction{Up, Down, Left, Right};
	
public:
	Game();
	
	void moveSnake();
	void changeDirection(Direction dir);
	void restart();
	void addBrick();
	void addBomb();
	void triggerBomb();
	void paint();
	int score();
	void die();
	void togglePause();
	bool isDead();
	void restoreDefaults();
	void setSelfCollision(bool flag);
	
public slots:
	
private:
	void setDefaults();
	
	// list of snake points in box units (allways positive, >=0)
	// head is first element
	QList<QPoint> snake; 
	bool dead = false;
	
	QList<QPoint> bricks; // filled randomly with obstackles
	QList<QPoint> bombs; // "food" which can be triggerd if needed in a tight situation
	QPoint lastBomb;

	int BombRadius;
	int SizeX; // boxes
	int SizeY; // boxes
	int BrickSize;// pixel
	int BrickAttraction; // number of retries for finding neighbours, before settling into nowhere
	int HeadClearance; // zone around head, forbidden for new bricks
	int InitialLength;
	int GrowInterval;
	int BombInterval;
	
	QColor BombColor;
	QColor SnakeColor1;
	QColor SnakeColor2;
	QColor SnakeHeadColor;
	QColor BackgroundColor;
	QColor BrickColor;
	QColor TextColor;
	
	QStringList valueNames;
	QList<int*> values;
	
	Direction currentHeadDirection=Right;
	QList<Direction> steerQueue;
	int bombsCounter=0;
	
	bool paused = false;
	bool selfCollision = false; 
	const int BombFadout = 5;
	int bombFadoutCounter=BombFadout;

};

#endif // SCENE_HH
