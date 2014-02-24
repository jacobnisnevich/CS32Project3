#include "Actor.h"
#include "StudentWorld.h"
#include <time.h>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// GameObject
GameObject::GameObject(StudentWorld* World, int image, int startx, int starty) : GraphObject(image, startx, starty)
{
	m_world = World;
	m_alive = true;
	m_player = false;
}

GameObject::~GameObject()
{
}

bool GameObject::isAlive() const
{
	return m_alive;
}

bool GameObject::isPlayer() const
{
	return m_player;
}

void GameObject::setAlive(bool life)
{
	m_alive = life;
}

void GameObject::setPlayer(bool player)
{
	m_player = player;
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

	// return false if hits brick
	DestructBrick* destructbrick = dynamic_cast<DestructBrick*>(object);
	if (destructbrick)
	{
		return false;
	}
	PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
	if (permabrick)
	{
		return false;
	}

	// kill player if hits Zumi
	Player* player = dynamic_cast<Player*>(this);
	SimpleZumi* simpzumi = dynamic_cast<SimpleZumi*>(object);
	if (simpzumi && player)
	{
		this->setAlive(false);
	}
	ComplexZumi* compzumi = dynamic_cast<ComplexZumi*>(object);
	if (compzumi && player)
	{
		this->setAlive(false);
	}
	return true;
}

// Player
Player::Player(StudentWorld* World, int startx, int starty) : Character(World, IID_PLAYER, startx, starty)
{
	setPlayer(true);
	setVisible(true);
}

Player::~Player()
{
}

void Player::doSomething()
{
	int keyPress;
	getWorld()->getKey(keyPress);
	if (keyPress != KEY_PRESS_SPACE)
		move(keyPress);
	else
		dropBugSprayer();
}

void Player::dropBugSprayer()
{
	if (getWorld()->getNumSprayers() > 1)
		return;
	getWorld()->getActors()->push_back(new BugSprayer(getWorld(), getX(), getY()));
	getWorld()->setNumSprayers(getWorld()->getNumSprayers() + 1);
}

// Zumi
Zumi::Zumi(StudentWorld* World, int image, int x, int y, int ticksPerMove) : Character(World, image, x, y)
{
	m_ticksPerMove = ticksPerMove;
	m_ticks = 0;
}

int Zumi::getRandomDirection()
{
	srand(time(NULL));
	return (rand() % 4 + 1);
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

	Player* player = dynamic_cast<Player*>(findObject(getX(), getY()));
	if (player)
	{
		findObject(getX(), getY())->setAlive(false);
	}
}

// Complex Zumi
ComplexZumi::ComplexZumi(StudentWorld* World, int x, int y, int ticksPerMove) : Zumi(World, IID_SIMPLE_ZUMI, x, y, ticksPerMove)
{
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
	setAlive(false);
	m_complete = false;
}

void Exit::doSomething()
{
	if (isVisible() && !isAlive())
	{
		setAlive(true);
	}
	for (int i = 0; i < (int)getWorld()->getActors()->size(); i++)
	{
		Player* player = dynamic_cast<Player*>(getWorld()->getActors()->at(i));
		if (player &&
			getWorld()->getActors()->at(i)->getX() == getX() && 
			getWorld()->getActors()->at(i)->getY() == getY() &&
			isAlive())
		{
			getWorld()->increaseScore(getWorld()->getLevelBonus());
			getWorld()->playSound(SOUND_FINISHED_LEVEL);
			m_complete = true;
		}
	}
}

// BugSprayer
BugSprayer::BugSprayer(StudentWorld* World, int x, int y) : Object(World, IID_BUGSPRAYER, x, y)
{
	setVisible(true);
	setAlive(true);
	setTime(40);
}

BugSprayer::~BugSprayer()
{
}

void BugSprayer::doSomething()
{
	if (isAlive() == false)
		return;

	decreaseTime();

	if (getTime() == 0)
	{		
		setAlive(false);
		setVisible(false);
		getWorld()->playSound(SOUND_SPRAY);
		getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY()));
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX() + i, getY());
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			if ((object && !permabrick))
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() + i, getY()));
				break;
			}
			else if (!object)
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() + i, getY()));
			else
				break;
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX() - i, getY());
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			if ((object && !permabrick))
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() - i, getY()));
				break;
			}
			else if (!object)
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX() - i, getY()));
			else
				break;
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX(), getY() + i);
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			if ((object && !permabrick))
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() + i));
				break;
			}
			else if (!object)
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() + i));
			else
				break;
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX(), getY() - i);
			PermaBrick* permabrick = dynamic_cast<PermaBrick*>(object);
			if ((object && !permabrick))
			{
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() - i));
				break;
			}
			else if (!object)
				getWorld()->getActors()->push_back(new BugSpray(getWorld(), getX(), getY() - i));
			else
				break;
		}
	}
}

// BugSpray
BugSpray::BugSpray(StudentWorld* World, int x, int y) : Object(World, IID_BUGSPRAY, x, y)
{
	setVisible(true);
	setAlive(true);
	setTime(3);
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
			object->setAlive(false);
		}
		ComplexZumi* compzumi = dynamic_cast<ComplexZumi*>(object);
		if (compzumi)
		{
			object->setAlive(false);
		}
		BugSprayer* bugsprayer = dynamic_cast<BugSprayer*>(object);
		if (bugsprayer)
		{
			object->setTime(0);
		}
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