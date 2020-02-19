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
	
	const int sizeX = 100; // boxes
	const int sizeY = 50; // boxes
	const int boxSize = 15; // pixel
	
	Direction currentHeadDirection=Right;
	QList<Direction> steerQueue;
	int bombsCounter=0;
};

#endif // SCENE_HH
