#include "game.hh"

#include <qgraphicsitem.h>
#include <QSettings>
#include <math.h>

Game::Game()
{
	BombColor = QColor(Qt::yellow);
	SnakeColor1 = QColor(100, 255, 100);
	SnakeColor2 = QColor(255,100,100);
	SnakeHeadColor = Qt::red;
	BackgroundColor = QColor(50,50,50);
	BrickColor = Qt::gray;
	TextColor = Qt::white;
	
	setBackgroundBrush(BackgroundColor);
	
	setDefaults();
	QSettings set;
	
	valueNames << "ExplosionRadius";
	valueNames << "SizeX";
	valueNames << "SizeY";
	valueNames << "BrickSize";
	valueNames << "BrickAttraction";
	valueNames << "HeadClearance";
	valueNames << "InitialLength";
	valueNames << "GrowInterval";
	valueNames << "BombInterval";
	
	values.append(&ExplosionRadius);
	values.append(&SizeX);
	values.append(&SizeY);
	values.append(&BrickSize);
	values.append(&BrickAttraction);
	values.append(&HeadClearance);
	values.append(&InitialLength);
	values.append(&GrowInterval);
	values.append(&BombInterval);
	values.append(&BrickInterval);
	
	for(int i=0; i<valueNames.length(); i++){
		if(set.contains(valueNames[i])){
			*(values[i]) = set.value(valueNames[i]).toDouble();
		}else{
			set.setValue(valueNames[i], *(values[i]));
		}
	}
	
	// TODO remove temporary:
	restoreDefaults();
	
	restart();
}

void Game::timeStep(qreal dt)
{
	if(dead || paused){
		paint();
		return;
	}
	
	// process steering
	qreal roundsPerSecond = 0.6; 
	if(currentSteering == Left){
		currentHeadDirection += 360*dt*roundsPerSecond;
		if(currentHeadDirection >= 360){
			currentHeadDirection -= 360;
		}
	}else if(currentSteering == Right){
		currentHeadDirection -= 360*dt*roundsPerSecond;
		if(currentHeadDirection<0){
			currentHeadDirection+=360;
		}
	}
	
	static int cnt=0;
	
	QPointF newHead;
	qreal s = speed * (boost?2:1);
	qreal dx = s*dt*cos(currentHeadDirection*M_PI/180.0);
	qreal dy = -s*dt*sin(currentHeadDirection*M_PI/180.0);
	QPointF d(dx,dy);
	if((cnt/8)%2 == 0){
		d *= 0.5; // set to zero for creep simulation
	}
	
	newHead = snake.first() + d;
	
	
	// check for self collision
	if(selfCollision){
		QList<QPointF> subList = snake.mid(20);
		if(isColliding(subList, newHead, SnakeSize) >= 0){
			die();
			return;
		}
	}
	// collision with bricks:
	for(int i=0; i<bricks.length(); i++){
		if(isColliding(bricks, newHead, (SnakeSize/2 + BrickSize/2)*0.8) >= 0){
			die();
			return;
		}
	}

	
	
	static qreal brickTimer=0;
	brickTimer += dt;
	if(brickTimer >= BrickInterval){
		brickTimer = 0;
		addBrick();
	}

	snake.insert(0, newHead);	
	static qreal growTimer=0;
	growTimer += dt;
	if(growTimer >= GrowInterval){
		growTimer = 0;
	}else{
		snake.removeLast();
	}
	
	static qreal bombTimer=0;
	bombTimer += dt;
	if(bombTimer >= BombInterval){
		bombTimer = 0;
		addBomb();
	}
	

	// collision with bombs
	int i = isColliding(bombs, newHead, BrickSize/2+SnakeSize/2);
	if(i >= 0){
		bombsCounter++;
		bombs.removeAt(i);
	}

	
	paint();
	
	cnt++;
}

void Game::controlInput(Game::Controlls dir)
{
	if(paused){
		togglePause();
	}
	
	switch(dir){
	case Boost: boost = true; break;
	case NormalSpeed: boost = false; break;
	case Left: 
	case Right: 
	case Straight: currentSteering = dir;
	}
	
}

void Game::restart()
{
	dead = false;
	snake.clear();
	bombsCounter = 10;
	bombFadoutCounter=BombFadeout;
	bombs.clear();
	lastBomb = QPointF();
	currentHeadDirection = 0;
	
	for(int i=0; i<InitialLength; i++){
		snake.insert(0,QPointF(SnakeSize*(i+5),BrickSize*SizeY/2));
	}
	
	bricks.clear();
	for(int x=0; x<=SizeX; x++){
		bricks.insert(0,QPointF(x, 0)*BrickSize);
		bricks.insert(0,QPointF(x, SizeY)*BrickSize);
	}
	for(int y=1; y<SizeY; y++){
		bricks.insert(0,QPointF(0,y)*BrickSize);
		bricks.insert(0,QPointF(SizeX,y)*BrickSize);
	}
	paused = true;
}

void Game::addBrick()
{
	int neighbourSearchTrials = int(BrickAttraction); // countdown for searching neighbours, if zero, we will plant a hermit in the middle of nowhere
	
	int retries = 1000;
	while(retries--){
		QPointF p((rand()%(int(SizeX*BrickSize-BrickSize)))+BrickSize, 
				 (rand()%(int(SizeY*BrickSize-BrickSize)))+BrickSize);
		
		// check for enough distance to head, to avoid unfair obstacles
		if(p.x()*p.x() + p.y()*p.y() < (HeadClearance*BrickSize)*(HeadClearance*BrickSize)){
			continue;
		}
		
		// check random position against snake, bricks and bombs
		if(isColliding(snake, p, (SnakeSize/2+BrickSize/2)*1.2) >= 0 ||
				isColliding(bricks, p, BrickSize*1.2) >= 0 ||
				isColliding(bombs, p, BrickSize*1.2) >= 0){
			continue;
		}
		
		// search for neigbours
		int found=0;
		qreal r=BrickSize*2; // search radius
		
		if(isColliding(bricks, p, r) >= 0){
			found = 1;
		}
		
		if(found >= 1 || neighbourSearchTrials<=0 ){
			bricks.append(p);
			break;
		}
		neighbourSearchTrials--;
	}
}

void Game::addBomb()
{
	int retries = 1000;
	while(retries--){
		QPointF p((rand()%(int(SizeX*BrickSize-BrickSize)))+BrickSize, 
				 (rand()%(int(SizeY*BrickSize-BrickSize)))+BrickSize);
		
		// check random position against snake, bricks and bombs
		if(isColliding(snake, p, (SnakeSize/2+BrickSize/2)*1.2) >= 0 ||
				isColliding(bricks, p, BrickSize*1.2) >= 0 ||
				isColliding(bombs, p, BrickSize*1.2) >= 0){
			continue;
		}

		bombs.append(p);
		break;
	}
}

void Game::triggerBomb()
{
	if(dead || paused)
		return;
	
	qreal r=ExplosionRadius;
	
	if(bombsCounter > 0){
		for(int i=0; i<bricks.length(); i++){
			QPointF p = bricks[i];
			if(round(p.x()) == 0 || round(p.x()) == SizeX*BrickSize || 
					round(p.y())==0 || round(p.y())==SizeY*BrickSize){
				continue;
			}
			

			if(isColliding(snake.first(), p, ExplosionRadius*BrickSize)){
				bricks.removeAt(i);
				i--;
			}
		}		
		bombsCounter--;
		lastBomb = snake.first();
		bombFadoutCounter = BombFadeout;
	}
	paint(); // immediately paint the bomb explosion
}

void Game::paint()
{
	clear();
	
	// Bricks
	for(int i=0; i<bricks.length(); i++){
		QPointF p = bricks[i];
		QBrush brush = QBrush(BrickColor);
		
		auto item = new QGraphicsEllipseItem(p.x()-0.5*BrickSize, p.y()-0.5*BrickSize, 
										  BrickSize, BrickSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Bombs
	for(int i=0; i<bombs.length(); i++){
		QPointF p = bombs[i];
		QBrush brush = QBrush(BombColor);
		
		auto item = new QGraphicsEllipseItem(p.x()-0.5*BrickSize, p.y()-0.5*BrickSize, 
											 BrickSize, BrickSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Snake
	for(int i=snake.length()-1; i>=0; i--){
		QPointF p = snake[i];
		QBrush brush(SnakeColor1);
		
		if(i==0){
			brush = QBrush(SnakeHeadColor);
		}else if(i <= bombsCounter){
			// collected bombs
			brush = QBrush(BombColor);
		}else if((i/10)%2 == 0){
			// stripes of the snake
			brush = QBrush(SnakeColor2);
		}
		auto item = new QGraphicsEllipseItem(p.x()-0.65*SnakeSize, p.y()-0.65*SnakeSize, 
											 SnakeSize*1.3, SnakeSize*1.3);
		item->setBrush(brush);
		addItem(item);
	}
	
	// explosion radius
	if(lastBomb.isNull() == false){
		QColor c = BombColor;
		c.setAlpha(128*bombFadoutCounter/BombFadeout);
		QBrush brush(c);
		
		qreal d = BrickSize*ExplosionRadius;
		auto item = new QGraphicsEllipseItem(lastBomb.x()-d, lastBomb.y()-d, 
											 2*d, 2*d);
		item->setBrush(brush);
		addItem(item);
		bombFadoutCounter--;
		
		if(bombFadoutCounter == 0){
			lastBomb = QPointF();
			bombFadoutCounter = BombFadeout;
		}
	}
	
	// dark overlay
	if(dead || paused){
		QBrush brush(QColor(0,0,0,128)); // transparent black as text background color
		auto rect = new QGraphicsRectItem(-BrickSize*0.5,-BrickSize*0.5,
										  BrickSize*(SizeX+1), BrickSize*(SizeY+1));
		rect->setBrush(brush);
		addItem(rect);
		
	}
	// Print Score
	if(dead){
		{
			auto brush = QBrush(TextColor);
			auto item = new QGraphicsSimpleTextItem(QString("SCORE %1").arg(score()));
			item->setBrush(brush);
			item->setFont(QFont("DejaVu Sans Mono, Bold", 64, 5));
			QRectF bR = item->sceneBoundingRect();
			item->setPos(QPointF(BrickSize*SizeX/2 - int(bR.width()/2), 
								BrickSize*SizeY/2 - int(bR.height()/2)));
			addItem(item);
		}
		{
			auto brush = QBrush(TextColor);
			auto item = new QGraphicsSimpleTextItem(QString("Press R to restart\n    ESC to quit"));
			item->setBrush(brush);
			item->setFont(QFont("DejaVu Sans Mono, Bold", 16, 5));
			QRectF bR = item->sceneBoundingRect();
			item->setPos(QPointF(BrickSize*SizeX/2 - int(bR.width()/2), BrickSize*(SizeY-1) - bR.height()));
			addItem(item);
		}
	}
	
	// Print Pause
	if(paused && !dead){
		auto brush = QBrush(TextColor);
		auto item = new QGraphicsSimpleTextItem(QString("PAUSED"));
		item->setBrush(brush);
		item->setFont(QFont("DejaVu Sans Mono, Bold", 64, 5));
		QRectF bR = item->sceneBoundingRect();
		item->setPos(QPointF(BrickSize*SizeX/2 - bR.width()/2, 
							BrickSize*SizeY/2 - bR.height()/2));
		addItem(item);
		
		{
			auto brush = QBrush(TextColor);
			auto item = new QGraphicsSimpleTextItem(QString("Press any cursor key to start\n        ESC to quit"));
			item->setBrush(brush);
			item->setFont(QFont("DejaVu Sans Mono, Bold", 16, 5));
			QRectF bR = item->sceneBoundingRect();
			item->setPos(QPointF(BrickSize*SizeX/2 - bR.width()/2, 
								 BrickSize*(SizeY-1) - bR.height()));
			addItem(item);
		}
	}
}

int Game::score()
{
	return snake.length();
}

void Game::die()
{
	dead = true;
	paint();
}

void Game::togglePause()
{
	if(dead){
		paused = false;
	}else{
		paused = !paused;
	}
	paint();
}

void Game::setPause(bool flag)
{
	paused = flag;
}

bool Game::isDead()
{
	return dead;
}

void Game::restoreDefaults()
{
	QSettings set;
	die();
	
	setDefaults();
	
	for(int i=0; i<valueNames.length(); i++){
		set.setValue(valueNames[i], *(values[i]));
	}
	
	restart();
}

void Game::setSelfCollision(bool flag)
{
	selfCollision = flag;
}

void Game::setDefaults()
{
	ExplosionRadius=12; // brick units
	SizeX = 60; // brick units
	SizeY = 40; // brick units
	BrickSize = 16; // pixels
	SnakeSize = 16; // pixels
	BrickAttraction = 15; // number of retries for finding neighbours, before settling into nowhere
	HeadClearance = 7; // zone around head, forbidden for new bricks
	InitialLength = 40;	
	GrowInterval = 1.0; // grow snake every n seconds
	BombInterval = 3.0; // spawn a bomb on the field every n seconds
	BrickInterval = 1.0; // a brick every second
}

bool Game::isColliding(QPointF &p1, QPointF &p2, qreal radius)
{
	QPointF d = p1-p2;
	if(d.x()*d.x() + d.y()*d.y() < radius*radius){
		return true;
	}else{
		return false;
	}
}

int Game::isColliding(QList<QPointF> &list, QPointF &p2, qreal radius)
{
	for(int i=0; i<list.length(); i++){
		if(isColliding(list[i], p2, radius)){
			return i;
		}
	}
	return -1;
}
