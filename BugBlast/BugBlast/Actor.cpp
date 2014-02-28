#include "Actor.h"
#include "StudentWorld.h"
#include <time.h>
#include <queue>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// GameObject
GameObject::GameObject(StudentWorld* World, int image, int startx, int starty) : GraphObject(image, startx, starty)
{
	m_world = World;
	m_alive = true;
}

GameObject::~GameObject()
{
}

// return if the object is alive
bool GameObject::isAlive() const
{
	return m_alive;
}

// set the object's alive state
void GameObject::setAlive(bool life)
{
	m_alive = life;
}

// Character
Character::Character(StudentWorld* World, int image, int startx, int starty) : GameObject(World, image, startx, starty)
{
}

// primary move function for all Characterse
bool Character::move(int dir)
{
	// get keyinput and call respective collision and moveTo functions
	switch (dir)
	{
	case KEY_PRESS_UP:
		if (!isCollision(getX(), getY() + 1))
			return false;
		moveTo(getX(), getY() + 1);
		break;
	case KEY_PRESS_DOWN:
		if (!isCollision(getX(), getY() - 1))
			return false;
		moveTo(getX(), getY() - 1);
		break;
	case KEY_PRESS_LEFT:
		if (!isCollision(getX() - 1, getY()))
			return false;
		moveTo(getX() - 1, getY());
		break;
	case KEY_PRESS_RIGHT:
		if (!isCollision(getX() + 1, getY()))
			return false;
		moveTo(getX() + 1, getY());
		break;
	default:
		break;
	}
	
	// return true by default
	return true;
}

bool Character::isCollision(int x, int y)
{
	// if it is null return
	GameObject* object = findObject(x, y);
	if (object == nullptr)
	{
		return true;
	}

	// if player has walkthrough power up let it walk through
	Player* playerThis = dynamic_cast<Player*>(this);
	DestructBrick* destructbrick = dynamic_cast<DestructBrick*>(object);
	if (destructbrick && playerThis && playerThis->getWalkThrough())
	{
		return true;
	}
	// return false if hits brick
	else if (destructbrick)
	{
		return false;
	}
	PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
	if (permabrick)
	{
		return false;
	}

	SimpleZumi* simpzumiThis = dynamic_cast<SimpleZumi*>(this);
	ComplexZumi* compzumiThis = dynamic_cast<ComplexZumi*>(this);
	
	// zumis can't walk through sprayers
	BugSprayer* sprayer = dynamic_cast<BugSprayer*>(object);
	if ((simpzumiThis || compzumiThis) && sprayer)
	{
		return false;
	}

	// kill player if zumi hits it
	Player* playerObject = dynamic_cast<Player*>(object);
	if ((simpzumiThis || compzumiThis) && playerObject)
	{
		playerObject->setAlive(false);
	}
	// kill player if hits Zumi
	SimpleZumi* simpzumiObject = dynamic_cast<SimpleZumi*>(object);
	if (simpzumiObject && playerThis)
	{
		this->setAlive(false);
	}
	ComplexZumi* compzumiObject = dynamic_cast<ComplexZumi*>(object);
	if (compzumiObject && playerThis)
	{
		this->setAlive(false);
	}

	return true;
}

// Player
Player::Player(StudentWorld* World, int startx, int starty) : Character(World, IID_PLAYER, startx, starty)
{
	setVisible(true);
	setWalkThrough(false);
	setIncreaseSprayers(false);
}

Player::~Player()
{
}

void Player::doSomething()
{
	// get user input for keypress
	int keyPress;
	getWorld()->getKey(keyPress);

	GameObject* object = findSprayerObject(getX(), getY());
	BugSprayer* sprayer = dynamic_cast<BugSprayer*>(object);

	// if user presses key other than space move, otherwise drop sprayer
	if (keyPress != KEY_PRESS_SPACE)
		move(keyPress);
	// if a spayer exists at the player's position do not drop a sprayer
	else if (keyPress == KEY_PRESS_SPACE && !sprayer)
		dropBugSprayer();

	// adjust the power up times anbd effects appropriately
	if (getWalkThroughTime() == 0)
		setWalkThrough(false);
	if (getWalkThrough())
		decreaseWalkThroughTime();
	if (getSprayerTime() == 0)
	{
		setSprayerTime(false);
		getWorld()->setMaxSprayer(getWorld()->getMaxSprayer() - (getWorld()->getMaxBoostedSprayers() - 2));
	}
	if (getIncreaseSprayers())
		decreaseSprayerTime();
}

// creates new bugsprayer at player's postion
void Player::dropBugSprayer()
{
	// if it limit, return immediately
	if (getWorld()->getNumSprayers() >= getWorld()->getMaxSprayer())
		return;
	
	// create new sprayer and increment count
	getWorld()->getActors()->push_back(new BugSprayer(getWorld(), getX(), getY()));
	getWorld()->setNumSprayers(getWorld()->getNumSprayers() + 1);
}

// Zumi
Zumi::Zumi(StudentWorld* World, int image, int x, int y, int ticksPerMove) : Character(World, image, x, y)
{
	m_ticksPerMove = ticksPerMove;
	m_ticks = 1;
	m_currentDirection = getRandomDirection();
}

// get random number between 1000 and 1003 inclusive
int Zumi::getRandomDirection()
{
	return (rand() % 4 + 1000);
}

// zumi death function
void Zumi::damage(int score)
{
	// set the Zumi to be dead
	setAlive(false);
	// play correct sound and increase score
	getWorld()->playSound(SOUND_ENEMY_DIE);
	getWorld()->increaseScore(score);
	// find random number from 0 to 100
	int totalProb = rand() % 101;
	// drop appropriate goodie
	if (totalProb < getWorld()->getGoodieProb())
	{
		if (totalProb < getWorld()->getExtraLifeProb())
			getWorld()->getActors()->push_back(new ExtraLife(getWorld(), getX(), getY()));
		else if (totalProb >= getWorld()->getExtraLifeProb() && 
			totalProb < (getWorld()->getExtraLifeProb() + getWorld()->getWalkThruProb()))
			getWorld()->getActors()->push_back(new WalkThrough(getWorld(), getX(), getY()));
		else if (totalProb >= (getWorld()->getExtraLifeProb() + getWorld()->getWalkThruProb()) && 
			totalProb < 100)
			getWorld()->getActors()->push_back(new IncreaseSprayer(getWorld(), getX(), getY()));
	}
}

// Simple Zumi
SimpleZumi::SimpleZumi(StudentWorld* World, int x, int y, int ticksPerMove) : Zumi(World, IID_SIMPLE_ZUMI, x, y, ticksPerMove)
{
	setVisible(true);
	setCurrentDirection(getRandomDirection());
}

void SimpleZumi::doSomething()
{
	// if its dead return
	if (!isAlive())
		return;
	
	// kill player if he is on same position
	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		object->setAlive(false);
	}

	// make move in current direction or pick a new move
	if (getTicks() == getTicksPerMove())
	{
		bool success = move(getCurrentDirection());
		if (!success)
			setCurrentDirection(getRandomDirection());
		// reset ticks
		setTicks(0);
	}
	// increment ticks
	else 
	{
		setTicks(getTicks() + 1);
	}
}

// Complex Zumi
ComplexZumi::ComplexZumi(StudentWorld* World, int x, int y, int ticksPerMove) : Zumi(World, IID_COMPLEX_ZUMI, x, y, ticksPerMove)
{
	setVisible(true);
	setCurrentDirection(getRandomDirection());
}

void ComplexZumi::doSomething()
{
	// if dead return
	if (!isAlive())
		return;
	
	// kill player if he is on same position
	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		findObject(getX(), getY())->setAlive(false);
	}

	// if the Zumi should move this turn
	if (getTicks() == getTicksPerMove())
	{
		// check if player is within smell distance
		for (int i = 0; i < getWorld()->getActors()->size(); i++)
		{
			Player* player = dynamic_cast<Player*>(getWorld()->getActors()->at(i));
			if (player)
			{
				int horizDistance = abs(player->getX() - getX()); 
				int vertDistance = abs(player->getY() - getY());
				int smellDistance = getWorld()->getComplexZumiSearchDistance();

				if (horizDistance <= smellDistance && vertDistance <= smellDistance)
				{
					// call breadth-first search function
					int dir = search(getX(), getY(), player->getX(), player->getY());
					// if result is valid move there and return
					if (dir != -1);
					{
						move(dir);
						setTicks(0);
						return;
					}
				}
				break;
			}
		}
		// if search failed, move in current direction or random new one
		bool success = move(getCurrentDirection());
		if (!success)
			setCurrentDirection(getRandomDirection());
		// reset number of ticks
		setTicks(0);
	}
	else 
	{
		// increment number of ticks
		setTicks(getTicks() + 1);
	}
}

// class for breadth-first search queue
class Coord
{
public:
	Coord(int xx, int yy, int dir) : m_x(xx), m_y(yy), m_dir(dir) {}
	int x() const { return m_x; }
	int y() const { return m_y; }
	int dir() const {return m_dir;}
private:
	int m_x;
	int m_y;
	int m_dir;
};

int ComplexZumi::search(int startX, int startY, int playerX, int playerY)
{
	// Create a queue of coordinates
	std::queue<Coord> moves;
	// Create a 15 by 15 char array to represent the maze
	char maze[VIEW_WIDTH][VIEW_HEIGHT];

	// Push the start coordinates and an invalid direction into the queue
	Coord start(startX, startY, 0);
	moves.push(start);
	// Mark the start with a # to represent bread crumbs
	maze[startX][startY] = '#';

	// While the queue is not empty
	while (!moves.empty())
	{
		// Pop the first coordinate
		Coord tempCoord = moves.front();
		moves.pop();
		// If the coordinate’s position is the position of the player
		if (tempCoord.x() == playerX && tempCoord.y() == playerY)
			// Return the direction
			return tempCoord.dir();
		// If the coordinate to the left is not a brick and the array is not marked
		if (!isBrick(tempCoord.x() - 1,tempCoord.y()) && maze[tempCoord.x() - 1][tempCoord.y()] != '#' &&
			tempCoord.x() - 1 >= 0 && tempCoord.x() - 1 <= 15 && tempCoord.y() >= 0 && tempCoord.y() <= 15)
		{
			// Mark the array with a #
			maze[tempCoord.x() - 1][tempCoord.y()] = '#';
			// If the coordinates direction is invalid
			if (tempCoord.dir() == 0)
			{
				// Push the coordinate to the queue with the left direction
				Coord pushCoord(tempCoord.x() - 1, tempCoord.y(), KEY_PRESS_LEFT);
				moves.push(pushCoord);
			}
			else
			{
				// Push the coordinate to the queue with its current direction
				Coord pushCoord(tempCoord.x() - 1, tempCoord.y(), tempCoord.dir());
				moves.push(pushCoord);
			}
		}
		// If the coordinate to the top is not a brick and the array is not marked
		if (!isBrick(tempCoord.x(),tempCoord.y() + 1) && maze[tempCoord.x()][tempCoord.y() + 1] != '#' &&
			tempCoord.x() >= 0 && tempCoord.x() <= 15 && tempCoord.y() + 1 >= 0 && tempCoord.y() + 1 <= 15)
		{
			// Mark the array with a #
			maze[tempCoord.x()][tempCoord.y() + 1] = '#';
			// If the coordinates direction is invalid
			if (tempCoord.dir() == 0)
			{
				// Push the coordinate to the queue with the up direction
				Coord pushCoord(tempCoord.x(), tempCoord.y() + 1, KEY_PRESS_UP);
				moves.push(pushCoord);
			}
			else
			{
				// Push the coordinate to the queue with its current direction
				Coord pushCoord(tempCoord.x(), tempCoord.y() + 1, tempCoord.dir());
				moves.push(pushCoord);
			}
		}
		// If the coordinate to the right is not a brick and the array is not marked
		if (!isBrick(tempCoord.x() + 1,tempCoord.y()) && maze[tempCoord.x() + 1][tempCoord.y()] != '#' &&
			tempCoord.x() + 1 >= 0 && tempCoord.x() + 1 <= 15 && tempCoord.y() >= 0 && tempCoord.y() <= 15)
		{
			// Mark the array with a #
			maze[tempCoord.x()+1][tempCoord.y()] = '#';
			// If the coordinates direction is invalid
			if (tempCoord.dir() == 0)
			{
				// Push the coordinate to the queue with the right direction
				Coord pushCoord(tempCoord.x() + 1, tempCoord.y(), KEY_PRESS_RIGHT);
				moves.push(pushCoord);
			}
			else
			{
				// Push the coordinate to the queue with its current direction
				Coord pushCoord(tempCoord.x() + 1, tempCoord.y(), tempCoord.dir());
				moves.push(pushCoord);
			}
		}
		// If the coordinate to the bottom is not a brick and the array is not marked
		if (!isBrick(tempCoord.x(),tempCoord.y() + 1) && maze[tempCoord.x()][tempCoord.y() - 1] != '#' &&
			tempCoord.x() >= 0 && tempCoord.x() <= 15 && tempCoord.y() + 1 >= 0 && tempCoord.y() + 1 <= 15)
		{
			// Mark the array with a #
			maze[tempCoord.x()][tempCoord.y() - 1] = '#';
			// If the coordinates direction is invalid
			if (tempCoord.dir() == 0)
			{
				// Push the coordinate to the queue with the down direction
				Coord pushCoord(tempCoord.x(), tempCoord.y() - 1, KEY_PRESS_DOWN);
				moves.push(pushCoord);
			}
			else
			{
				// Push the coordinate to the queue with its current direction
				Coord pushCoord(tempCoord.x(), tempCoord.y() - 1, tempCoord.dir());
				moves.push(pushCoord);
			}
		}
	}

	// Return -1 to indicate an invalid direction
	return -1;
}

// Object
Object::Object(StudentWorld* World, int image, int x, int y) : GameObject(World, image, x, y)
{
}

// Brick
Brick::Brick(StudentWorld* World, int image, int x, int y) : Object(World, image, x, y)
{
}

// PermaBrick
PermaBrick::PermaBrick(StudentWorld* World, int x, int y) : Brick(World, IID_PERMA_BRICK, x, y)
{
	setVisible(true);
}

// DestructBrick
DestructBrick::DestructBrick(StudentWorld* World, int x, int y) : Brick(World, IID_DESTROYABLE_BRICK, x, y)
{
	setVisible(true);
}

// Exit
Exit::Exit(StudentWorld* World, int x, int y) : Object(World, IID_EXIT, x, y)
{
	setVisible(false);
	setActive(false);
	m_complete = false;
}

void Exit::doSomething()
{
	// if it is visible but not active set it to be active
	if (isVisible() && !isActive())
	{
		setActive(true);
	}
	// if player steps on exit and the exit is active, set it as complete
	for (int i = 0; i < (int)getWorld()->getActors()->size(); i++)
	{
		Player* player = dynamic_cast<Player*>(getWorld()->getActors()->at(i));
		if (player &&
			getWorld()->getActors()->at(i)->getX() == getX() && 
			getWorld()->getActors()->at(i)->getY() == getY() &&
			isActive())
		{
			getWorld()->increaseScore(getWorld()->getLevelBonus());
			getWorld()->playSound(SOUND_FINISHED_LEVEL);
			m_complete = true;
		}
	}
}

// BugSprayer
BugSprayer::BugSprayer(StudentWorld* World, int x, int y) : TempObject(World, IID_BUGSPRAYER, x, y, 40)
{
	setVisible(true);
	setAlive(true);
}

BugSprayer::~BugSprayer()
{
}

void BugSprayer::doSomething()
{
	// If the BugSprayer is not alive return
	if (isAlive() == false)
		return;

	// Decrement its lifetime by 1
	decreaseTime();

	// If the lifetime is less than or equal to zero detonate it
	if (getTime() <= 0)
	{		
		setAlive(false);
		setVisible(false);
		getWorld()->playSound(SOUND_SPRAY);
		// Create up to 2 BugSprays in each direction, 
		// stop immediately if it hits a PermaBrick, 
		// if it hits a DestructBrick, 
		// do not place any more in this direction
		getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY()));
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX() + i, getY());
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			DestructBrick* destructbrick = dynamic_cast<DestructBrick*>(object);
			if (destructbrick)
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() + i, getY()));
				break;
			}
			else if (permabrick)
				break;
			else
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() + i, getY()));
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX() - i, getY());
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			DestructBrick* destructbrick = dynamic_cast<DestructBrick*>(object);
			if (destructbrick)
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() - i, getY()));
				break;
			}
			else if (permabrick)
				break;
			else
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() - i, getY()));
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX(), getY() + i);
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			DestructBrick* destructbrick = dynamic_cast<DestructBrick*>(object);
			if (destructbrick)
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() + i));
				break;
			}
			else if (permabrick)
				break;
			else
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() + i));
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX(), getY() - i);
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			DestructBrick* destructbrick = dynamic_cast<DestructBrick*>(object);
			if (destructbrick)
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() - i));
				break;
			}
			else if (permabrick)
				break;
			else
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() - i));
		}
	}
}

// BugSpray
BugSpray::BugSpray(StudentWorld* World, int x, int y) : TempObject(World, IID_BUGSPRAY, x, y, 3)
{
	setVisible(true);
	setAlive(true);
}

void BugSpray::doSomething()
{
	// If the BugSpray is not alive, return
	if (!isAlive())
		return;

	// Decrement its lifetime by 1
	decreaseTime();

	// If the lifetime is equal to zero kill it
	if (getTime() == 0)
	{
		setAlive(false);
		setVisible(false);
	}

	// if the spray is on a player, a zumi, a destructible brick kill it
	// if the spray is on a bugsprayer, detonate it next tick
	GameObject* object = findObject(getX(), getY());
	if (object)
	{
		DestructBrick* destructbrick = dynamic_cast<DestructBrick*>(object);
		if (destructbrick)
		{
			object->setAlive(false);
		}
		Player* player = dynamic_cast<Player*>(object);
		if (player)
		{
			object->setAlive(false);
		}
		SimpleZumi* simpzumi = dynamic_cast<SimpleZumi*>(object);
		if (simpzumi)
		{
			simpzumi->damage(100);
		}
		ComplexZumi* compzumi = dynamic_cast<ComplexZumi*>(object);
		if (compzumi)
		{
			compzumi->damage(500);
		}
		BugSprayer* bugsprayer = dynamic_cast<BugSprayer*>(object);
		if (bugsprayer)
		{
			bugsprayer->setTime(0);
		}
	}
}

TempObject::TempObject(StudentWorld* World, int image, int x, int y, int time) : Object(World, image, x, y)
{
	setTime(time);
}

ExtraLife::ExtraLife(StudentWorld* World, int x, int y) 
	: TempObject(World, IID_EXTRA_LIFE_GOODIE, x, y, World->getGoodieLifetime())
{
	setVisible(true);
}

void ExtraLife::doSomething()
{
	// if its not alive return
	if (!isAlive())
		return;

	// decrease lifetime by 1
	decreaseTime();

	// if lifetime runs out, kill it
	if (getTime() == 0)
		setAlive(false);

	// if the goodie is on a player
	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		// increase lives and score, kill the goodie, play the sound
		getWorld()->incLives();
		getWorld()->increaseScore(1000);
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		return;
	}
}

WalkThrough::WalkThrough(StudentWorld* World, int x, int y) 
	: TempObject(World, IID_WALK_THRU_GOODIE, x, y, World->getGoodieLifetime())
{
	setVisible(true);
}

void WalkThrough::doSomething()
{
	// if its not alive return
	if (!isAlive())
		return;

	// decrease lifetime by 1
	decreaseTime();

	// if lifetime runs out, kill it
	if (getTime() == 0)
		setAlive(false);

	// if the goodie is on a player
	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		// set the players walkthrough and give it a lifetime,
		// kill the goodie, play the sound
		player->setWalkThrough(true);
		player->setWalkThroughTime(getWorld()->getWalkThruLifetimeTicks());
		getWorld()->increaseScore(1000);
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		return;
	}
}

IncreaseSprayer::IncreaseSprayer(StudentWorld* World, int x, int y) 
	: TempObject(World, IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE, x, y, World->getGoodieLifetime())
{
	setVisible(true);
}

void IncreaseSprayer::doSomething()
{
	// if its not alive retur
	if (!isAlive())
		return;

	// decrease lifetime by 1
	decreaseTime();

	// if lifetime runs out, kill it
	if (getTime() == 0)
		setAlive(false);

	// if the goodie is on a player
	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		// set the world's max number of sprayers and give it a lifetime,
		// kill the goodie, play the sound
		player->setIncreaseSprayers(true);
		getWorld()->setMaxSprayer(getWorld()->getMaxSprayer() + getWorld()->getMaxBoostedSprayers());
		player->setSprayerTime(getWorld()->getBoostedSprayerLifetimeTicks());
		getWorld()->increaseScore(1000);
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		return;
	}
}

GameObject* GameObject::findObject(int x, int y)
{
	// loop through actors array
	for (int i = 0; i < (int)getWorld()->getActors()->size(); i++)
	{
		if (getWorld()->getActors()->at(i)->getX() == x &&
			getWorld()->getActors()->at(i)->getY() == y)
		{
			// return a pointer to the actor if its coordinates match parameters
			return getWorld()->getActors()->at(i);
		}
	}
	return nullptr;
}

GameObject* GameObject::findSprayerObject(int x, int y)
{
	// loop through actors array
	for (int i = 0; i < (int)getWorld()->getActors()->size(); i++)
	{
		BugSprayer* bugsprayer = dynamic_cast<BugSprayer*>(getWorld()->getActors()->at(i));

		if (getWorld()->getActors()->at(i)->getX() == x &&
			getWorld()->getActors()->at(i)->getY() == y && bugsprayer)
		{
			// return a pointer to the actor if its coordinates match parameters
			// and the actor is a sprayer
			return getWorld()->getActors()->at(i);
		}
	}
	return nullptr;
}

bool GameObject::isEmpty(int x, int y)
{
	// find the object at  x, y
	GameObject* object = findObject(x, y);
	// if there is no object return true
	if (!object)
		return true;
	else 
		return false;
}

bool GameObject::isPlayer(int x, int y)
{
	// find the object at  x, y
	GameObject* object = findObject(x, y);
	Player* player = dynamic_cast<Player*>(object);
	// if it is a player
	if (player)
		return true;
	else
		return false;
}

bool GameObject::isBrick(int x, int y)
{
	// find the object at  x, y
	GameObject* object = findObject(x, y);
	Brick* brick = dynamic_cast<Brick*>(object);
	// if it is a brick
	if (brick)
		return true;
	else
		return false;
}