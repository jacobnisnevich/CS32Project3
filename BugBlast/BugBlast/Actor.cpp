#include "Actor.h"
#include "StudentWorld.h"

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
	if (object->getID() == IID_DESTROYABLE_BRICK || object->getID() == IID_PERMA_BRICK)
	{
		return false;
	}
	else if ((object->getID() == IID_SIMPLE_ZUMI || object->getID() == IID_COMPLEX_ZUMI) && this->getID() == IID_PLAYER)
	{
		this->setAlive(false);
		return true;
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
	for (int i = 0; i < (int)m_sprayers.size(); i++)
	{
		delete m_sprayers[i];
	}
	m_sprayers.clear();
}

void Player::doSomething()
{
	int keyPress;
	getWorld()->getKey(keyPress);
	if (keyPress != KEY_PRESS_SPACE)
		move(keyPress);
	else
		dropBugSprayer();

	for (int i = 0; i < (int)m_sprayers.size(); i++)
	{
		m_sprayers[i]->doSomething();
		if (!m_sprayers[i]->isAlive())
		{
			delete m_sprayers[i];
			m_sprayers.erase(m_sprayers.begin() + i);
			break;
		}
	}
}

void Player::dropBugSprayer()
{
	if (m_sprayers.size() > 1)
		return;
	m_sprayers.push_back(new BugSprayer(getWorld(), getX(), getY()));
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
	if (isVisible())
	{
		setAlive(true);
	}
	for (int i = 0; i < (int)getWorld()->getActors().size(); i++)
	{
		if (getWorld()->getActors()[i]->getID() == IID_PLAYER &&
			getWorld()->getActors()[i]->getX() == getX() && 
			getWorld()->getActors()[i]->getY() == getY() &&
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
	for (int i = 0; i < (int)m_sprays.size(); i++)
	{
		delete m_sprays[i];
	}
	m_sprays.clear();
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
		m_sprays.push_back(new BugSpray(getWorld(), getX(), getY()));
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX() + i, getY());
			if ((object && object->getID() != IID_PERMA_BRICK))
			{
				m_sprays.push_back(new BugSpray(getWorld(), getX() + i, getY()));
				break;
			}
			else if (!object)
				m_sprays.push_back(new BugSpray(getWorld(), getX() + i, getY()));
			else
				break;
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX() - i, getY());
			if (object && object->getID() != IID_PERMA_BRICK)
			{
				m_sprays.push_back(new BugSpray(getWorld(), getX() - i, getY()));
				break;
			}
			else if (!object)
				m_sprays.push_back(new BugSpray(getWorld(), getX() - i, getY()));
			else
				break;
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX(), getY() + i);
			if (object && object->getID() != IID_PERMA_BRICK)
			{
				m_sprays.push_back(new BugSpray(getWorld(), getX(), getY() + i));
				break;
			}
			else if (!object)
				m_sprays.push_back(new BugSpray(getWorld(), getX(), getY() + i));
			else
				break;
		}
		for (int i = 1; i <= 2; i++)
		{
			GameObject* object = findObject(getX(), getY() - i);
			if (object && object->getID() != IID_PERMA_BRICK)
			{
				m_sprays.push_back(new BugSpray(getWorld(), getX(), getY() - i));
				break;
			}
			else if (!object)
				m_sprays.push_back(new BugSpray(getWorld(), getX(), getY() - i));
			else
				break;
		}
	}

	for (int i = 0; i < (int)m_sprays.size(); i++)
	{
		m_sprays[i]->doSomething();
		if (!m_sprays[i]->isAlive())
		{
			delete m_sprays[i];
			m_sprays.erase(m_sprays.begin() + i);
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
		if (object->getID() == IID_DESTROYABLE_BRICK ||
			object->getID() == IID_PLAYER ||
			object->getID() == IID_SIMPLE_ZUMI ||
			object->getID() == IID_COMPLEX_ZUMI)
		{
			object->setAlive(false);
		}
		else if (object->getID() == IID_BUGSPRAYER)
		{
			object->setTime(0);
		}
	}
}

GameObject* GameObject::findObject(int x, int y)
{
	for (int i = 0; i < (int)getWorld()->getActors().size(); i++)
	{
		if (getWorld()->getActors()[i]->getX() == x &&
			getWorld()->getActors()[i]->getY() == y)
		{
			return getWorld()->getActors()[i];
		}
	}
	return nullptr;
}