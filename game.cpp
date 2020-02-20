#include "game.hh"

#include <qgraphicsitem.h>
#include <QSettings>

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
	
	valueNames << "BombRadius";
	valueNames << "SizeX";
	valueNames << "SizeY";
	valueNames << "BrickSize";
	valueNames << "BrickAttraction";
	valueNames << "HeadClearance";
	valueNames << "InitialLength";
	valueNames << "GrowInterval";
	valueNames << "BombInterval";
	
	values.append(&BombRadius);
	values.append(&SizeX);
	values.append(&SizeY);
	values.append(&BrickSize);
	values.append(&BrickAttraction);
	values.append(&HeadClearance);
	values.append(&InitialLength);
	values.append(&GrowInterval);
	values.append(&BombInterval);

	for(int i=0; i<valueNames.length(); i++){
		if(set.contains(valueNames[i])){
			*(values[i]) = set.value(valueNames[i]).toInt();
		}else{
			set.setValue(valueNames[i], *(values[i]));
		}
	}
	
	restart();
}

void Game::moveSnake()
{
	if(dead || paused){
		paint();
		return;
	}
	
	static int cnt=0;
	
	if(steerQueue.empty() == false){
		currentHeadDirection = steerQueue.first();
		steerQueue.removeFirst();
	}
	
	QPoint newHead;
	switch(currentHeadDirection){
	case Up:
		newHead.setX(snake.first().x());
		newHead.setY(snake.first().y()-1);
		break;
	case Down:
		newHead.setX(snake.first().x());
		newHead.setY(snake.first().y()+1);
		break;
	case Left:
		newHead.setX(snake.first().x()-1);
		newHead.setY(snake.first().y());
		break;
	case Right:
		newHead.setX(snake.first().x()+1);
		newHead.setY(snake.first().y());
		break;
	}
	
	
	// check for collision
	if((snake.contains(newHead) && selfCollision) ||
			bricks.contains(newHead)){
		die();
		return;
	}
	
	snake.insert(0, newHead);
	if(cnt%GrowInterval != 0){
		snake.removeLast();
	}
	addBrick();
	if(cnt%BombInterval == 0){
		addBomb();
	}
	
	
	//   with bombs
	if(bombs.contains(newHead)){
		bombsCounter++;
		bombs.removeOne(newHead);
	}
	
	
	paint();
	
	cnt++;
}

void Game::changeDirection(Game::Direction dir)
{
	if(paused){
		togglePause();
	}
	
	Direction lastDir;
	if(steerQueue.length() >= 1){
		lastDir = steerQueue.last();
	}else{
		lastDir = currentHeadDirection;
	}
	
	if((dir == Left && lastDir != Right) ||
			(dir == Right && lastDir != Left) ||
			(dir == Up && lastDir != Down) ||
			(dir == Down && lastDir != Up)
			){
		if(dir != lastDir){
			steerQueue.append(dir);
		}
	}
}

void Game::restart()
{
	dead = false;
	snake.clear();
	bombsCounter = 0;
	bombFadoutCounter=BombFadout;
	bombs.clear();
	lastBomb = QPoint();
	
	for(int i=0; i<InitialLength; i++){
		snake.insert(0,QPoint(i+1,SizeY/2));
	}
	
	steerQueue.clear();
	steerQueue.append(Right);
	bricks.clear();
	for(int x=0; x<SizeX; x++){
		bricks.insert(0,QPoint(x,0));
		bricks.insert(0,QPoint(x,SizeY));
	}
	for(int y=0; y<SizeY; y++){
		bricks.insert(0,QPoint(0,y));
		bricks.insert(0,QPoint(SizeX,y));
	}
	paused = true;
}

void Game::addBrick()
{
	int neighbourSearchTrials = BrickAttraction; // countdown for searching neighbours, if zero, we will plant a hermit in the middle of nowhere
	
	int retries = 1000;
	while(retries--){
		QPoint p((rand()%(SizeX-1))+1, 
				 (rand()%(SizeY-1))+1);
		
		// check for enough distance to head, to avoid unfair obstacles
		if(abs(p.x()-snake.first().x()) <= HeadClearance &&
				abs(p.y()-snake.first().y()) <= HeadClearance){
			continue;
		}
		
		// check random position against snake, bricks and bombs
		if(snake.contains(p) ||
				bricks.contains(p) ||
				bombs.contains(p)){
			continue;
		}
		
		// search for neigbours
		int found=0;
		const int r=1; // search radius
		
		for(int i=-r; i<=r; i++){
			for(int j=-r; j<=r; j++){
				if(i!=0 && j!=0){ // skip center and diagonals
					if(bricks.contains(QPoint(p.x()+i,p.y()+j))){
						found++;
					}
				}
			}
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
		QPoint p((rand()%(SizeX-1))+1, 
				 (rand()%(SizeY-1))+1);
		
		// check random position against snake, bricks and bombs
		if(snake.contains(p) == false &&
				bricks.contains(p) == false &&
				bombs.contains(p) == false){
			bombs.append(p);
			break;
		}
	}
}

void Game::triggerBomb()
{
	if(dead || paused)
		return;
	
	int r=BombRadius;
	
	if(bombsCounter > 0){
		for(int i=0; i<bricks.length(); i++){
			QPoint p = bricks[i];
			if(p.x() == 0 || p.x() == SizeX || p.y()==0 || p.y()==SizeY){
				continue;
			}
			
			int dx = snake.first().x() - p.x();
			int dy = snake.first().y() - p.y();
			if(dx*dx + dy*dy <= r*r){
				bricks.removeAt(i);
				i--;
			}
		}		
		bombsCounter--;
		lastBomb = snake.first();
		bombFadoutCounter = BombFadout;
	}
	paint(); // immediately paint the bomb explosion
}

void Game::paint()
{
	clear();
	
	// Bricks
	for(int i=0; i<bricks.length(); i++){
		QPoint p = bricks[i];
		QBrush brush = QBrush(BrickColor);
		
		auto item = new QGraphicsRectItem(BrickSize*p.x(), BrickSize*p.y(), BrickSize, BrickSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Bombs
	for(int i=0; i<bombs.length(); i++){
		QPoint p = bombs[i];
		QBrush brush = QBrush(BombColor);
		
		auto item = new QGraphicsEllipseItem(BrickSize*p.x(), BrickSize*p.y(), BrickSize, BrickSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Snake
	for(int i=snake.length()-1; i>=0; i--){
		QPoint p = snake[i];
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
		auto item = new QGraphicsEllipseItem(BrickSize*(p.x()-0.15), BrickSize*(p.y()-0.15), BrickSize*1.3, BrickSize*1.3);
		item->setBrush(brush);
		item->setPen(QPen(QBrush(Qt::black), BrickSize/6.0));
		addItem(item);
	}
	
	// explosion radius
	if(lastBomb.isNull() == false){
		QColor c = BombColor;
		c.setAlpha(128*bombFadoutCounter/BombFadout);
		QBrush brush(c);
		
		int d = BrickSize*BombRadius;
		auto item = new QGraphicsEllipseItem(BrickSize*lastBomb.x()-d, BrickSize*lastBomb.y()-d, 
											 BrickSize*2*BombRadius, BrickSize*2*BombRadius);
		item->setBrush(brush);
		addItem(item);
		bombFadoutCounter--;
		
		if(bombFadoutCounter == 0){
			lastBomb = QPoint();
			bombFadoutCounter = BombFadout;
		}
	}
	
	// dark overlay
	if(dead || paused){
		QBrush brush(QColor(0,0,0,128)); // transparent black as text background color
		auto rect = new QGraphicsRectItem(0,0,BrickSize*(SizeX+1), BrickSize*(SizeY+1));
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
			item->setPos(QPoint(BrickSize*SizeX/2 - int(bR.width()/2), 
								BrickSize*SizeY/2 - int(bR.height()/2)));
			addItem(item);
		}
		{
			auto brush = QBrush(TextColor);
			auto item = new QGraphicsSimpleTextItem(QString("Press R to restart\n    ESC to quit"));
			item->setBrush(brush);
			item->setFont(QFont("DejaVu Sans Mono, Bold", 16, 5));
			QRectF bR = item->sceneBoundingRect();
			item->setPos(QPoint(BrickSize*SizeX/2 - int(bR.width()/2), BrickSize*(SizeY-1) - bR.height()));
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
		item->setPos(QPoint(BrickSize*SizeX/2 - int(bR.width()/2), 
							BrickSize*SizeY/2 - int(bR.height()/2)));
		addItem(item);
		
		{
			auto brush = QBrush(TextColor);
			auto item = new QGraphicsSimpleTextItem(QString("Press any cursor key to start\n        ESC to quit"));
			item->setBrush(brush);
			item->setFont(QFont("DejaVu Sans Mono, Bold", 16, 5));
			QRectF bR = item->sceneBoundingRect();
			item->setPos(QPoint(BrickSize*SizeX/2 - int(bR.width()/2), BrickSize*(SizeY-1) - bR.height()));
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
	BombRadius=10;
	SizeX = 60; // boxes
	SizeY = 40; // boxes
	BrickSize = 14; // pixel
	BrickAttraction = 15; // number of retries for finding neighbours, before settling into nowhere
	HeadClearance = 7; // zone around head, forbidden for new bricks
	InitialLength = 10;	
	GrowInterval = 10; // grow snake every 10 steps
	BombInterval = 30; // spawn a bomb on the field every 100 steps
}
