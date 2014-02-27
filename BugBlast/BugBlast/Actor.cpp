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

bool GameObject::isAlive() const
{
	return m_alive;
}

void GameObject::setAlive(bool life)
{
	m_alive = life;
}

// Character
Character::Character(StudentWorld* World, int image, int startx, int starty) : GameObject(World, image, startx, starty)
{
}

bool Character::move(int dir)
{
	Level collision;
	collision.loadLevel(getWorld()->getLevelFile(getWorld()->getLevel()));

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
	return true;
}

bool Character::isCollision(int x, int y)
{
	GameObject* object = findObject(x, y);
	if (object == nullptr)
	{
		return true;
	}

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
	int keyPress;
	getWorld()->getKey(keyPress);

	GameObject* object = findSprayerObject(getX(), getY());
	BugSprayer* sprayer = dynamic_cast<BugSprayer*>(object);

	if (keyPress != KEY_PRESS_SPACE)
		move(keyPress);
	else if (keyPress == KEY_PRESS_SPACE && !sprayer)
		dropBugSprayer();

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

void Player::dropBugSprayer()
{
	if (getWorld()->getNumSprayers() >= getWorld()->getMaxSprayer())
		return;
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

int Zumi::getRandomDirection()
{
	return (rand() % 4 + 1000);
}

void Zumi::damage(int score)
{
	setAlive(false);
	getWorld()->playSound(SOUND_ENEMY_DIE);
	getWorld()->increaseScore(score);
	int totalProb = rand() % 101;
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
	if (!isAlive())
		return;
	
	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		object->setAlive(false);
	}

	if (getTicks() == getTicksPerMove())
	{
		bool success = move(getCurrentDirection());
		if (!success)
			setCurrentDirection(getRandomDirection());
		setTicks(0);
	}
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
	if (!isAlive())
		return;
	
	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		findObject(getX(), getY())->setAlive(false);
	}

	if (getTicks() == getTicksPerMove())
	{
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
					int dir = search(getX(), getY(), player->getX(), player->getY());
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
		bool success = move(getCurrentDirection());
		if (!success)
			setCurrentDirection(getRandomDirection());
		setTicks(0);
	}
	else 
	{
		setTicks(getTicks() + 1);
	}
}

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
	std::queue<Coord> moves;
	char maze[VIEW_WIDTH][VIEW_HEIGHT];

	Coord start(startX, startY, 0);
	moves.push(start);
	maze[startX][startY] = '#';

	while (!moves.empty())
	{
		Coord tempCoord = moves.front();
		moves.pop();
		if (tempCoord.x() == playerX && tempCoord.y() == playerY)
			return tempCoord.dir();
		if (!isBrick(tempCoord.x() - 1,tempCoord.y()) && maze[tempCoord.x() - 1][tempCoord.y()] != '#')
		{
			maze[tempCoord.x() - 1][tempCoord.y()] = '#';
			if (tempCoord.dir() == 0)
			{
				Coord pushCoord(tempCoord.x() - 1, tempCoord.y(), KEY_PRESS_LEFT);
				moves.push(pushCoord);
			}
			else
			{
				Coord pushCoord(tempCoord.x() - 1, tempCoord.y(), tempCoord.dir());
				moves.push(pushCoord);
			}
		}
		if (!isBrick(tempCoord.x(),tempCoord.y() + 1) && maze[tempCoord.x()][tempCoord.y() + 1] != '#')
		{
			maze[tempCoord.x()][tempCoord.y() + 1] = '#';
			if (tempCoord.dir() == 0)
			{
				Coord pushCoord(tempCoord.x(), tempCoord.y() + 1, KEY_PRESS_UP);
				moves.push(pushCoord);
			}
			else
			{
				Coord pushCoord(tempCoord.x(), tempCoord.y() + 1, tempCoord.dir());
				moves.push(pushCoord);
			}
		}
		if (!isBrick(tempCoord.x() + 1,tempCoord.y()) && maze[tempCoord.x() + 1][tempCoord.y()] != '#')
		{
			maze[tempCoord.x()+1][tempCoord.y()] = '#';
			if (tempCoord.dir() == 0)
			{
				Coord pushCoord(tempCoord.x() + 1, tempCoord.y(), KEY_PRESS_RIGHT);
				moves.push(pushCoord);
			}
			else
			{
				Coord pushCoord(tempCoord.x() + 1, tempCoord.y(), tempCoord.dir());
				moves.push(pushCoord);
			}
		}
		if (!isBrick(tempCoord.x(),tempCoord.y() + 1)
			&& maze[tempCoord.x()][tempCoord.y() - 1] != '#')
		{
			maze[tempCoord.x()][tempCoord.y() - 1] = '#';
			if (tempCoord.dir() == 0)
			{
				Coord pushCoord(tempCoord.x(), tempCoord.y() - 1, KEY_PRESS_DOWN);
				moves.push(pushCoord);
			}
			else
			{
				Coord pushCoord(tempCoord.x(), tempCoord.y() - 1, tempCoord.dir());
				moves.push(pushCoord);
			}
		}
	}

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
	if (isVisible() && !isActive())
	{
		setActive(true);
	}
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
	if (isAlive() == false)
		return;

	decreaseTime();

	if (getTime() <= 0)
	{		
		setAlive(false);
		setVisible(false);
		getWorld()->playSound(SOUND_SPRAY);
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
	if (!isAlive())
		return;

	decreaseTime();

	if (getTime() == 0)
	{
		setAlive(false);
		setVisible(false);
	}

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
	if (!isAlive())
		return;

	decreaseTime();

	if (getTime() == 0)
		setAlive(false);

	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
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
	if (!isAlive())
		return;

	decreaseTime();

	if (getTime() == 0)
		setAlive(false);

	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
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
	if (!isAlive())
		return;

	decreaseTime();

	if (getTime() == 0)
		setAlive(false);

	GameObject* object = findObject(getX(), getY());
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
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
	for (int i = 0; i < (int)getWorld()->getActors()->size(); i++)
	{
		if (getWorld()->getActors()->at(i)->getX() == x &&
			getWorld()->getActors()->at(i)->getY() == y)
		{
			return getWorld()->getActors()->at(i);
		}
	}
	return nullptr;
}

GameObject* GameObject::findSprayerObject(int x, int y)
{
	for (int i = 0; i < (int)getWorld()->getActors()->size(); i++)
	{
		BugSprayer* bugsprayer = dynamic_cast<BugSprayer*>(getWorld()->getActors()->at(i));

		if (getWorld()->getActors()->at(i)->getX() == x &&
			getWorld()->getActors()->at(i)->getY() == y && bugsprayer)
		{
			return getWorld()->getActors()->at(i);
		}
	}
	return nullptr;
}

bool GameObject::isEmpty(int x, int y)
{
	GameObject* object = findObject(x, y);
	if (!object)
		return true;
	else 
		return false;
}

bool GameObject::isPlayer(int x, int y)
{
	GameObject* object = findObject(x, y);
	Player* player = dynamic_cast<Player*>(object);
	if (player)
		return true;
	else
		return false;
}

bool GameObject::isBrick(int x, int y)
{
	GameObject* object = findObject(x, y);
	Brick* brick = dynamic_cast<Brick*>(object);
	if (brick)
		return true;
	else
		return false;
}