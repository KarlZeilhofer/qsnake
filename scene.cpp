#include "scene.hh"

#include <qgraphicsitem.h>

Scene::Scene()
{
	reset();
	setBackgroundBrush(QColor(Qt::black));
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
	
	snake.insert(0, newHead);
	if(cnt%10 != 0){
		snake.removeLast();
	}
	addBrick();
	if(cnt%100 == 0){
		addBomb();
	}

	// check for collision
	//   with snake
	for(int i=1; i<snake.length(); i++){
		if(newHead.x() == snake[i].x() &&
				newHead.y() == snake[i].y()){
			dead = true;
			return;
		}
	}
	//   with bricks
	if(bricks.contains(newHead)){
		dead = true;
		return;
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

	for(int i=0; i<15; i++){
		snake.insert(0,QPoint(i+sizeX/2,sizeY/2));
	}

	steerQueue.clear();
	steerQueue.append(Right);
	bricks.clear();
	for(int x=0; x<sizeX; x++){
		bricks.insert(0,QPoint(x,0));
		bricks.insert(0,QPoint(x,sizeY));
	}
	for(int y=0; y<sizeY; y++){
		bricks.insert(0,QPoint(0,y));
		bricks.insert(0,QPoint(sizeX,y));
	}
}

void Scene::addBrick()
{
	bool added = false;
	bool collision = false;
	int neighbourSearchTrials = 15; // countdown for searching neighbours, if zero, we will plant a hermit in the middle of nowhere
	while(!added){
		collision = false;
		int x = rand()%sizeX;
		int y = rand()%sizeY;
		
		// check for enough distance to head, to avoid unfair obstacles
		if(abs(x-snake.first().x()) < 7 &&
				abs(y-snake.first().y()) < 7){
			collision = true;
			continue;
		}
		
		// check random position against snake and bricks
		if(!collision)
		for(int i=0; i<snake.length(); i++){
			if(x == snake[i].x() && y == snake[i].y()){
				collision = true;
				break;
			}
		}
		
		if(!collision)
		for(int i=0; i<bricks.length(); i++){
			if(x == bricks[i].x() && y == bricks[i].y()){
				collision = true;
				break;
			}
		}
		
		// search for neigbours
		int found=0;
		const int r=1; // search radius
		
		if(!collision)
		for(int i=-r; i<=r; i++){
			for(int j=-r; j<=r; j++){
				if(i!=0 && j!=0){ // skip center and diagonals
					if(bricks.contains(QPoint(x+i,y+j))){
						found++;
					}
				}
			}
		}
		
		
		
		if(!collision && 
				(found >= 1 || neighbourSearchTrials<=0) ){
			bricks.append(QPoint(x,y));
			added=true;
		}
		neighbourSearchTrials--;
	}
}

void Scene::addBomb()
{
	bool added = false;
	bool collision = false;

	while(!added){
		collision = false;
		int x = rand()%sizeX;
		int y = rand()%sizeY;
		
		
		// check random position against snake and bricks
		if(!collision)
		for(int i=0; i<snake.length(); i++){
			if(x == snake[i].x() && y == snake[i].y()){
				collision = true;
				break;
			}
		}
		
		if(!collision)
		for(int i=0; i<bricks.length(); i++){
			if(x == bricks[i].x() && y == bricks[i].y()){
				collision = true;
				break;
			}
		}
		
		if(!collision)
		for(int i=0; i<bombs.length(); i++){
			if(x == bombs[i].x() && y == bombs[i].y()){
				collision = true;
				break;
			}
		}
		
		if(!collision){
			bombs.append(QPoint(x,y));
			added=true;
		}
	}
}

void Scene::triggerBomb()
{
	int r=BombRadius;
	
	if(bombsCounter > 0){
		for(int i=0; i<bricks.length(); i++){
			QPoint p = bricks[i];
			if(p.x() == 0 || p.x() == sizeX || p.y()==0 || p.y()==sizeY){
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
		auto item = new QGraphicsEllipseItem(boxSize*p.x(), boxSize*p.y(), boxSize, boxSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Bricks
	for(int i=0; i<bricks.length(); i++){
		QPoint p = bricks[i];
		QBrush brush = QBrush(Qt::gray);
		
		auto item = new QGraphicsRectItem(boxSize*p.x(), boxSize*p.y(), boxSize, boxSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// Bombs
	for(int i=0; i<bombs.length(); i++){
		QPoint p = bombs[i];
		QBrush brush = QBrush(QColor(150,150,255));
		
		auto item = new QGraphicsEllipseItem(boxSize*p.x(), boxSize*p.y(), boxSize, boxSize);
		item->setBrush(brush);
		addItem(item);
	}
	
	// explosion radius
	if(lastBomb.isNull() == false){
		QBrush brush(QBrush(QColor(150,150,255)));
		int d = boxSize*BombRadius;
		auto item = new QGraphicsEllipseItem(boxSize*lastBomb.x()-d, boxSize*lastBomb.y()-d, 
											 boxSize*2*BombRadius, boxSize*2*BombRadius);
		item->setBrush(brush);
		addItem(item);
		lastBomb = QPoint();
	}
}

int Scene::length()
{
	return snake.length();
}

bool Scene::isDead()
{
	return dead;
}
