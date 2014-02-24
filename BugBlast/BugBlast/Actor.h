#ifndef ACTOR_H_
#define ACTOR_H_

#include <vector>
#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;
class Level;
class BugSprayer;
class BugSpray;

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
	virtual bool isComplete() = 0;
	void decreaseTime() {m_lifetime--;}
	void setTime(int time) {m_lifetime = time;}
	int getTime() const {return m_lifetime;}
	GameObject* findObject(int x, int y);
private:
	StudentWorld* m_world;
	bool m_player;
	bool m_alive;
	int m_lifetime;
};

class Character : public GameObject
{
public:
	Character(StudentWorld* World, int image, int startx, int starty);
	virtual void doSomething() = 0;
	bool move(int dir);
	bool isCollision(int x, int y);
	virtual bool isComplete() {return false;}
};

class Player : public Character
{
public:
	Player(StudentWorld* World, int startx, int starty);
	virtual ~Player();
	void dropBugSprayer();
	virtual void doSomething();
};

class Zumi : public Character
{
public:
	Zumi(StudentWorld* World, int image, int x, int y, int ticksPerMove);
	virtual ~Zumi() {};
	int getTicksPerMove() const {return m_ticksPerMove;}
	int getTicks() const {return m_ticks;}
	void setTicks(int ticks) {m_ticks = ticks;}
	int getCurrentDirection() const {return m_currentDirection;}
	void setCurrentDirection(int dir) {m_currentDirection = dir;}
	int getRandomDirection();
private:
	int m_ticksPerMove;
	int m_ticks;
	int m_currentDirection;
};

class SimpleZumi : public Zumi
{
public:
	SimpleZumi(StudentWorld* World, int x, int y, int ticksPerMove);
	virtual ~SimpleZumi() {};
	virtual void doSomething();
};

class ComplexZumi : public Zumi
{
public:
	ComplexZumi(StudentWorld* World, int x, int y, int ticksPerMove);
	virtual ~ComplexZumi() {};
	virtual void doSomething();
};

class Object : public GameObject
{
public:
	Object(StudentWorld* World, int image, int x, int y);
	virtual bool isComplete() {return false;}
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
	virtual bool isComplete() {return m_complete;}
	bool m_complete;
};

class BugSprayer : public Object
{
public:
	BugSprayer(StudentWorld* World, int x, int y);
	virtual ~BugSprayer();
	virtual void doSomething();
};

class BugSpray : public Object
{
public:
	BugSpray(StudentWorld* World, int x, int y);
	virtual ~BugSpray() {};
	virtual void doSomething();
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
