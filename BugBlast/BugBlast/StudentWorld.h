#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld();
	virtual ~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	int getNumZumis() const;
	void setNumZumis(int num);
	std::string getLevelFile(unsigned int num);
private:
	// level options
	int ProbOfGoodieOverall;
	int ProbOfExtraLifeGoodie;
	int ProbOfWalkThruGoodie;
	int ProbOfMoreSprayersGoodie;
	int TicksPerSimpleZumiMove;
	int TicksPerComplexZumiMove;
	int GoodieLifetimeInTicks;
	int LevelBonus;
	int WalkThruLifetimeTicks;
	int BoostedSprayerLifetimeTicks;
	int MaxBoostedSprayers;

	// level objects
	std::vector<GameObject*> actors;
	int numZumis;
};

#endif // STUDENTWORLD_H_
