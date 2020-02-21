#ifndef SCENE_HH
#define SCENE_HH

#include <QGraphicsScene>

class Game : public QGraphicsScene
{
public:
	enum Controlls{Straight, Left=1, Right=2, NormalSpeed=4, Boost=8};
	
public:
	Game();
	
	void timeStep(qreal dt);
	void controlInput(Controlls dir);
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
	bool isColliding(QPointF& p1, QPointF& p2, qreal radius);
	int isColliding(QList<QPointF>& list, QPointF& p2, qreal radius);
	
	// list of snake points in box units (allways positive, >=0)
	// head is first element
	QList<QPointF> snake; 
	bool dead = false;
	
	QList<QPointF> bricks; // filled randomly with obstackles
	QList<QPointF> bombs; // "food" which can be triggerd if needed in a tight situation
	QPointF lastBomb;

	// settings
	qreal ExplosionRadius;
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
	Controlls currentSteering = Straight;

	int bombsCounter=0;
	
	bool paused = false;
	bool selfCollision = false; 
	const int BombFadeout = 20;
	int bombFadoutCounter=BombFadeout;
	qreal speed = 100; // px/s
	bool boost = false;
};

#endif // SCENE_HH
