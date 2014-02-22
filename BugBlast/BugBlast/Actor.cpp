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

	bool moveSuccess = true;

	switch (dir)
	{
	case KEY_PRESS_UP:
		onCollision(getX(), getY() + 1, collision, moveSuccess);
		if (moveSuccess == false)
			return moveSuccess;
		moveTo(getX(), getY() + 1);
		break;
	case KEY_PRESS_DOWN:
		onCollision(getX(), getY() - 1, collision, moveSuccess);
		if (moveSuccess == false)
			return moveSuccess;
		moveTo(getX(), getY() - 1);
		break;
	case KEY_PRESS_LEFT:
		onCollision(getX() - 1, getY(), collision, moveSuccess);
		if (moveSuccess == false)
			return moveSuccess;
		moveTo(getX() - 1, getY());
		break;
	case KEY_PRESS_RIGHT:
		onCollision(getX() + 1, getY(), collision, moveSuccess);
		if (moveSuccess == false)
			return moveSuccess;
		moveTo(getX() + 1, getY());
		break;
	default:
		break;
	}
	return true;
}

void Character::onCollision(int x, int y, Level collision, bool& success)
{
	if (collision.getContentsOf(x, y) == 5 || collision.getContentsOf(x, y) == 6)
	{
		success = false;
	}
	else if ((collision.getContentsOf(x, y) == 3 || collision.getContentsOf(x, y) == 4) && this->getID() == IID_PLAYER)
	{
		this->setAlive(false);
	}
}

// Player
Player::Player(StudentWorld* World, int startx, int starty) : Character(World, IID_PLAYER, startx, starty)
{
	m_sprayerCount = 0;
	setPlayer(true);
	setVisible(true);
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
	m_active = false;
	m_complete = false;
}

void Exit::doSomething()
{
	if (isVisible())
	{
		setActive(true);
	}
	for (int i = 0; i < getWorld()->getActors().size(); i++)
	{
		if (getWorld()->getActors()[i]->getID() == IID_PLAYER &&
			getWorld()->getActors()[i]->getX() == getX() && 
			getWorld()->getActors()[i]->getY() == getY() &&
			m_active == true)
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
	m_lifetime = 40;
}

void BugSprayer::doSomething()
{
	if (isAlive == false)
		return;

	decreaseTime();

	if (m_lifetime == 0)
	{
		// create bugspray
		getWorld()->playSound(SOUND_SPRAY);
		setAlive(false);		
	}
}