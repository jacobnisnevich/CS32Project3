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
	void onCollision(int x, int y, Level collision, bool& success);
	virtual bool isComplete() {return false;}
};

class Player : public Character
{
public:
	Player(StudentWorld* World, int startx, int starty);
	virtual ~Player();
	void dropBugSprayer();
	virtual void doSomething();
private:
	std::vector<BugSprayer*> m_sprayers;
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
	virtual bool isComplete() {return false;}
	virtual ~Object() {};
	void decreaseTime() {m_lifetime--;}
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
	virtual bool isComplete() {return m_complete;}
private:
	bool m_active;
	bool m_complete;
};

class BugSprayer : public Object
{
public:
	BugSprayer(StudentWorld* World, int x, int y);
	virtual ~BugSprayer() {};
	virtual void doSomething();
	void decreaseTime() {m_lifetime--;}
private:
	int m_lifetime;
	std::vector<BugSpray*> m_sprays;
};

class BugSpray : public Object
{
public:
	BugSpray(StudentWorld* World, int x, int y);
	virtual ~BugSprayer() {};
	virtual void doSomething();
	void decreaseTime() {m_lifetime--;}
private:
	int m_lifetime;
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
