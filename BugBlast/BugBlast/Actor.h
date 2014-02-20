#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

class GameObject : public GraphObject
{
public:
	GameObject(StudentWorld* World, int image, int startx, int starty);
	virtual ~GameObject();
	virtual bool isAlive() const;
	virtual void setAlive(bool life);
	virtual void doSomething() = 0;
	virtual bool isPlayer() const;
	virtual void setPlayer(bool player);
	StudentWorld* getWorld() const {return m_world;} 
private:
	StudentWorld* m_world;
	bool m_player;
	bool m_alive;
};

class Character : public GameObject
{
public:
	Character(StudentWorld* World, int image, int startx, int starty);
	virtual void doSomething() = 0;
	bool move(int dir);
};

class Player : public Character
{
public:
	Player(StudentWorld* World, int startx, int starty);
	virtual ~Player() {};
	void dropBugSprayer() {};
	virtual void doSomething();
private:
	int m_sprayerCount;
};

class Zumi : public Character
{
};

class SimpleZumi : public Zumi
{
};

class ComplexZumi : public Zumi
{
};

class Object : public GameObject
{
public:
	Object(StudentWorld* World, int image, int x, int y);
	virtual ~Object() {};
};

class Brick : public Object
{
public:
	Brick(StudentWorld* World, int image, int x, int y);
	virtual ~Brick() {};
	virtual void doSomething() {};
	
};

class PermaBrick : public Brick
{
public:
	PermaBrick(StudentWorld* World, int x, int y);
	virtual ~PermaBrick() {};
};

class DestructBrick : public Brick
{
public:
	DestructBrick(StudentWorld* World, int x, int y);
	virtual ~DestructBrick() {};
};

class Exit : public Object
{
public:
	Exit(StudentWorld* World, int x, int y);
	virtual ~Exit() {};
	virtual void doSomething();
	void setActive(bool active) {m_active = active;}
	bool getActive() const {return m_active;}
private:
	bool m_active;
};

class BugSprayer : public Object
{
};

class BugSpray : public Object
{
};

class Goody : public Object
{
};

class ExtraLife : public Goody
{
};

class WalkThrough : public Goody
{
};

class IncreaseSprayer : public Goody
{
};

#endif // ACTOR_H_
