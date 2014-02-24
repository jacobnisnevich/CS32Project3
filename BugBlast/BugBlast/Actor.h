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
	StudentWorld* getWorld() const {return m_world;} 
	virtual bool isComplete() = 0;
	GameObject* findObject(int x, int y);
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
	void setWalkThrough(bool canwalkthrough) {m_canWalkThrough = canwalkthrough;}
	bool getWalkThrough() {return m_canWalkThrough;}
	int getGoodyTime() {return m_walkThroughTime;}
	void setGoodyTime(int time) {m_walkThroughTime = time;}
	void decreaseGoodyTime() {m_walkThroughTime--;}
private:
	bool m_canWalkThrough;
	int m_walkThroughTime;
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
	void damage(int score);
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
	virtual void doSomething() {};
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
	bool isActive() {return m_active;}
	void setActive(bool active) {m_active = active;}
private:
	bool m_active;
	bool m_complete;
};

class TempObject : public Object
{
public:
	TempObject(StudentWorld* World, int image, int x, int y, int time);
	virtual ~TempObject() {};
	void decreaseTime() {m_lifetime--;}
	void setTime(int time) {m_lifetime = time;}
	int getTime() const {return m_lifetime;}
private:
	int m_lifetime;
};

class BugSprayer : public TempObject
{
public:
	BugSprayer(StudentWorld* World, int x, int y);
	virtual ~BugSprayer();
	virtual void doSomething();
};

class BugSpray : public TempObject
{
public:
	BugSpray(StudentWorld* World, int x, int y);
	virtual ~BugSpray() {};
	virtual void doSomething();
};

class ExtraLife : public TempObject
{
public:
	ExtraLife(StudentWorld* World, int x, int y);
	virtual ~ExtraLife() {};
	virtual void doSomething() {};
};

class WalkThrough : public TempObject
{
public:
	WalkThrough(StudentWorld* World, int x, int y);
	virtual ~WalkThrough() {};
	virtual void doSomething();
};

class IncreaseSprayer : public TempObject
{
public:
	IncreaseSprayer(StudentWorld* World, int x, int y);
	virtual ~IncreaseSprayer() {};
	virtual void doSomething() {};
};

#endif // ACTOR_H_
