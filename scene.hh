#ifndef SCENE_HH
#define SCENE_HH

#include <QGraphicsScene>

class Scene : public QGraphicsScene
{
public:
	enum Direction{Up, Down, Left, Right};
	
public:
	Scene();
	
	void moveSnake();
	void changeDirection(Direction dir);
	void reset();
	void addBrick();
	void addBomb();
	void triggerBomb();
	void paint();
	int length();
	void die();
	bool isDead();
	
public slots:
	
private:
	
	// list of snake points in box units (allways positive, >=0)
	// head is first element
	QList<QPoint> snake; 
	bool dead = false;
	
	QList<QPoint> bricks; // filled randomly with obstackles
	QList<QPoint> bombs; // "food" which can be triggerd if needed in a tight situation
	QPoint lastBomb;

	const int BombRadius=10;
	const int SizeX = 100; // boxes
	const int SizeY = 50; // boxes
	const int BoxSize = 15; // pixel
	const int BrickAttraction = 15; // number of retries for finding neighbours, before settling into nowhere
	const int HeadClearance = 7; // zone around head, forbidden for new bricks
	const int InitialLength = 15;
	
	Direction currentHeadDirection=Right;
	QList<Direction> steerQueue;
	int bombsCounter=0;
};

#endif // SCENE_HH
