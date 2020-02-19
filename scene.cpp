#include "scene.hh"

#include <qgraphicsitem.h>

Scene::Scene()
{
	reset();
	setBackgroundBrush(QColor(QColor(50,50,50)));
}

void Scene::moveSnake()
{
	if(dead){
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
	if(snake.contains(newHead) ||
			bricks.contains(newHead)){
		die();
		return;
	}
	
	snake.insert(0, newHead);
	if(cnt%10 != 0){
		snake.removeLast();
	}
	addBrick();
	if(cnt%100 == 0){
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

void Scene::changeDirection(Scene::Direction dir)
{
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

void Scene::reset()
{
	dead = false;
	snake.clear();
	bombsCounter = 0;
	bombs.clear();
	lastBomb = QPoint();
	
	for(int i=0; i<InitialLength; i++){
		snake.insert(0,QPoint(i+SizeX/2,SizeY/2));
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
}

void Scene::addBrick()
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

void Scene::addBomb()
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

void Scene::triggerBomb()
{
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
	}
}

void Scene::paint()
{
	clear();
	
	// Snake
	for(int i=0; i<snake.length(); i++){
		QPoint p = snake[i];
		QBrush brush(QColor(200, 255, 200));
		if(i==0){
			brush = QBrush(Qt::red);
		}else if(i <= bombsCounter){
			// stripes of the snake
			brush = QBrush(QColor(150,150,255));
		}else if(i%5 == 0){
			// stripes of the snake
			brush = QBrush(QColor(255,220,220));
		}
		auto item = new QGraphicsEllipseItem(BoxSize*p.x(), BoxSize*p.y(), BoxSize, BoxSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Bricks
	for(int i=0; i<bricks.length(); i++){
		QPoint p = bricks[i];
		QBrush brush = QBrush(Qt::gray);
		
		auto item = new QGraphicsRectItem(BoxSize*p.x(), BoxSize*p.y(), BoxSize, BoxSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Bombs
	for(int i=0; i<bombs.length(); i++){
		QPoint p = bombs[i];
		QBrush brush = QBrush(QColor(150,150,255));
		
		auto item = new QGraphicsEllipseItem(BoxSize*p.x(), BoxSize*p.y(), BoxSize, BoxSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// explosion radius
	if(lastBomb.isNull() == false){
		QBrush brush(QBrush(QColor(150,150,255)));
		int d = BoxSize*BombRadius;
		auto item = new QGraphicsEllipseItem(BoxSize*lastBomb.x()-d, BoxSize*lastBomb.y()-d, 
											 BoxSize*2*BombRadius, BoxSize*2*BombRadius);
		item->setBrush(brush);
		addItem(item);
		lastBomb = QPoint();
	}
	
	// Print Score
	if(dead){
		QBrush brush(Qt::white);
		auto item = new QGraphicsSimpleTextItem(QString("Score %1").arg(length()));
		item->setBrush(brush);
		item->setFont(QFont("DejaVu Sans Mono, Bold", 64, 5));
		QRectF bR = item->sceneBoundingRect();
		item->setPos(QPoint(BoxSize*SizeX/2 - int(bR.width()/2), 
							BoxSize*SizeY/2 - int(bR.height()/2)));
		addItem(item);
	}
}

int Scene::length()
{
	return snake.length();
}

void Scene::die()
{
	dead = true;
	paint();
}

bool Scene::isDead()
{
	return dead;
}
